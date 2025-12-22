# NileGo - IoT Firmware

ESP32 firmware for smart bicycle locks with GPS and Bluetooth.

## Hardware
- ESP32-DevKitC
- Neo-6M GPS Module
- 12V Solenoid Lock
- Li-ion Battery Pack

## Setup
1. Install Arduino IDE
2. Add ESP32 board support
3. Install libraries (see below)
4. Upload to ESP32

## Libraries Required
- TinyGPS++
- ArduinoBLE
- WiFi
- HTTPClient

## Pin Configuration
- GPS RX → GPIO 16
- GPS TX → GPIO 17
- Solenoid → GPIO 5
- Status LED → GPIO 2
