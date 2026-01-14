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

## Pin Configuration

- GPS RX → GPIO 16
- GPS TX → GPIO 17
- Solenoid → GPIO 5
- Status LED → GPIO 2


### **Documentation Folder Structure**

- requirements.md          - Project requirements
-  architecture.md          - System design
-  setup_guide.md          - How to run locally
-  api_documentation.md    - API endpoints (backend)
-  testing_plan.md         - Test cases
  
## Workflow

- Pull latest changes: git pull origin develop
- Create feature branch: git checkout -b feature/bluetooth-unlock
- Make changes and commit: git commit -m "Add Bluetooth unlock function"
- Push: git push origin feature/bluetooth-unlock
- Create Pull Request on GitHub
- Teammate reviews and merges
  
**Branch strategy:**

- `main` - production/demo ready code
- `develop` - integration branch
- `feature/gps-integration` - individual features
- `bugfix/lock-timeout` - bug fixes

---
