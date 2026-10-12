# Cold Room Monitoring System — CAN Bus & IoT

Academic project (2025–2026): ESP32-based monitoring of cold-room conditions, combining DHT22 temperature/humidity sensing, MQ-2 gas sensing, CAN communication through MCP2515, MQTT messaging, Node-RED, InfluxDB and Grafana.

## Repository status

- `firmware/sender/sender.ino`: cleaned, reconstructed sender sketch based on the code appendix in the report.
- `firmware/receiver/receiver.ino`: cleaned, reconstructed receiver sketch based on the code appendix.
- `docs/images/`: selected project diagrams, prototype and dashboard images extracted from the report.
- `requirements.txt`: included because requested; this project does not use Python packages.
- The sketches are **not hardware-validated**. Review the pin assignments, CAN module clock, wiring and MQTT broker settings before flashing.

## Hardware mentioned in the report

- ESP32 / NodeMCU-32S
- DHT22 sensor
- MQ-2 gas sensor
- MCP2515 CAN module
- Relay and fan
- Wi-Fi network and MQTT broker (Mosquitto)

## Arduino libraries

Install using Arduino IDE Library Manager (or the corresponding library source):
- WiFi (ESP32 board package)
- WiFiManager
- PubSubClient
- DHT sensor library
- Adafruit Unified Sensor (dependency commonly needed by DHT library)
- Seeed / Cory Fowler MCP_CAN library (for `mcp_can.h`; confirm the library version used in your setup)

## Setup

1. Install the ESP32 board package and the libraries above.
2. Open `firmware/sender/sender.ino` and `firmware/receiver/receiver.ino` as separate sketches and flash them to the appropriate boards.
3. Update `MQTT_BROKER` to the IP/hostname of your broker and check the MQTT port/topic settings.
4. Verify the MCP2515 oscillator frequency and CAN bitrate match your module and network.
5. Verify the relay pin against the actual wiring. The report's source uses GPIO 5 for both CAN chip-select and relay output; this is a pin conflict in the extracted design and must be resolved before use.
6. Start Mosquitto, Node-RED, InfluxDB and Grafana as configured in your environment.

## MQTT interface

- Sensor topic: `esp32/sensors`
- Fan command topic: `esp32/fan/control`
- Fan command payloads: `ON` or `OFF`

## Important note

The sketches are a cleaned reconstruction of the report appendix, not a verbatim source export. Check them against the original project and test safely before use.
