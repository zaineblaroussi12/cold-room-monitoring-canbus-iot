#include <WiFi.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <mcp_can.h>

#define CAN_CS_PIN 5
MCP_CAN CAN(CAN_CS_PIN);

const char* MQTT_BROKER = "192.168.1.10"; // Replace with your broker address.
const int MQTT_PORT = 1883;
const char* TOPIC_DATA = "esp32/sensors";

WiFiClient espClient;
PubSubClient client(espClient);

void reconnectMqtt() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT broker...");
    if (client.connect("ESP32Receiver")) {
      Serial.println("connected");
    } else {
      Serial.print("MQTT error, rc=");
      Serial.println(client.state());
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  WiFiManager wifiManager;
  wifiManager.autoConnect("ESP32-Config");

  Serial.print("Wi-Fi IP: ");
  Serial.println(WiFi.localIP());
  client.setServer(MQTT_BROKER, MQTT_PORT);

  while (CAN_OK != CAN.begin(MCP_ANY, CAN_125KBPS, MCP_8MHZ)) {
    Serial.println("CAN initialization failed; check oscillator/wiring.");
    delay(500);
  }
  CAN.setMode(MCP_NORMAL);
  Serial.println("CAN receiver ready");
}

void loop() {
  if (!client.connected()) reconnectMqtt();
  client.loop();

  if (CAN_MSGAVAIL != CAN.checkReceive()) return;

  unsigned long rxId = 0;
  byte len = 0;
  byte buf[8] = {0};
  CAN.readMsgBuf(&rxId, &len, buf);

  if (rxId != 0x101 || len < 4) return;

  const int temperature = buf[0];
  const int humidity = buf[1];
  const int gas = (buf[2] << 8) | buf[3];

  String payload = "{\"temperature\":" + String(temperature)
                 + ",\"humidity\":" + String(humidity)
                 + ",\"gas\":" + String(gas) + "}";
  client.publish(TOPIC_DATA, payload.c_str());
  Serial.println(payload);
}
