# Cold Room Monitoring System — CAN Bus & IoT

## Overview

An embedded IoT project designed to monitor cold-room conditions using an ESP32, CAN Bus communication and an IoT dashboard.

## Hardware

* ESP32 / NodeMCU-32S
* DHT22 temperature and humidity sensor
* MQ-2 gas sensor
* MCP2515 CAN Bus module
* Relay and fan

## Technologies

* C / Arduino
* CAN Bus
* MQTT (Mosquitto)
* Node-RED
* InfluxDB
* Grafana

## Features

* Temperature and humidity monitoring
* Gas sensor data acquisition
* CAN Bus communication
* MQTT messaging
* Dashboard visualization
* Fan control through a relay

## Project Structure

* `firmware/sender/`: ESP32 sender firmware
* `firmware/receiver/`: ESP32 receiver firmware
* `docs/images/`: project diagrams and screenshots

## Status

Academic project. The reconstructed firmware and hardware configuration require verification and testing before deployment.
