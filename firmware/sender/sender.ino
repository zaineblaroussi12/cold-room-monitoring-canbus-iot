#include <WiFi.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <SPI.h>
#include <mcp_can.h>
#include <math.h>

// Pins and hardware settings: verify against your actual wiring.
#define DHTPIN 14
#define DHTTYPE DHT22
#define MQ2PIN 34
#define CAN_CS_PIN 5
#define FAN_PIN 26  // TODO: confirm the relay pin from the actual wiring.

DHT dht(DHTPIN, DHTTYPE);
MCP_CAN CAN(CAN_CS_PIN);

const char* MQTT_BROKER = "192.168.1.10"; // Replace with your broker address.
const int MQTT_PORT = 1883;
const char* TOPIC_DATA = "esp32/sensors";
const char* TOPIC_FAN = "esp32/fan/control";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
bool fanState = false;

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) message += (char)payload[i];

  if (String(topic) == TOPIC_FAN) {
    if (message == "ON") {
      fanState = true;
      digitalWrite(FAN_PIN, HIGH);
    } else if (message == "OFF") {
      fanState = false;
      digitalWrite(FAN_PIN, LOW);
    }
    Serial.print("Fan state: ");
    Serial.println(fanState ? "ON" : "OFF");
  }
}

void reconnectMqtt() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT broker...");
    if (client.connect("ESP32Sender")) {
      Serial.println("connected");
      client.subscribe(TOPIC_FAN);
    } else {
      Serial.print("MQTT error, rc=");
      Serial.println(client.state());
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(FAN_PIN, OUTPUT);

  WiFiManager wifiManager;
  wifiManager.autoConnect("ESP32-Config");
  Serial.print("Wi-Fi IP: ");
  Serial.println(WiFi.localIP());

  client.setServer(MQTT_BROKER, MQTT_PORT);
  client.setCallback(callback);

  while (CAN_OK != CAN.begin(MCP_ANY, CAN_125KBPS, MCP_8MHZ)) {
    Serial.println("CAN initialization failed; check oscillator/wiring.");
    delay(500);
  }
  CAN.setMode(MCP_NORMAL);
  Serial.println("CAN sender ready");
}

void loop() {
  if (!client.connected()) reconnectMqtt();
  client.loop();

  const unsigned long now = millis();
  if (now - lastMsg < 5000) return;
  lastMsg = now;

  const float temperature = dht.readTemperature();
  const float humidity = dht.readHumidity();
  const int gas = analogRead(MQ2PIN);

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("DHT read error");
    return;
  }

  // The report encodes temperature and humidity as integer bytes.
  // This loses decimal precision; gas is encoded as two bytes.
  byte canData[4];
  canData[0] = (int)temperature;
  canData[1] = (int)humidity;
  canData[2] = (gas >> 8) & 0xFF;
  canData[3] = gas & 0xFF;

  const byte sendStatus = CAN.sendMsgBuf(0x101, 0, 4, canData);
  if (sendStatus != CAN_OK) Serial.println("CAN frame send failed");

  String payload = "{\"temperature\":" + String(temperature, 2)
                 + ",\"humidity\":" + String(humidity, 2)
                 + ",\"gas\":" + String(gas) + "}";
  client.publish(TOPIC_DATA, payload.c_str());
  Serial.println(payload);
}
