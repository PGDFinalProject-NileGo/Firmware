# NileGo - IoT Firmware

ESP32 firmware for smart bicycle locks with GPS and Bluetooth.

##Workflow
Pull latest changes: git pull origin develop
Create feature branch: git checkout -b feature/bluetooth-unlock
Make changes and commit: git commit -m "Add Bluetooth unlock function"
Push: git push origin feature/bluetooth-unlock
Create Pull Request on GitHub
Teammate reviews and merges


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





### Initial Commits
In firmware repo:
bashcd nilego-firmware
mkdir src docs
touch src/main.ino
touch docs/pin_diagram.md
touch README.md
git add .
git commit -m "Initial project structure"
git push origin main
In mobile repo:
bashcd nilego-mobile
flutter create nilego_app
cd nilego_app

# Edit README.md
git add .
git commit -m "Initial Flutter project"
git push origin main
In backend repo:
bashcd nilego-backend
mkdir firebase docs
touch docs/database_schema.md
touch README.md
git add .
git commit -m "Initial backend structure"
git push origin main


---

### **9. Documentation Folder Structure**

Add `/docs` to each repo:

/docs
  requirements.md          - Project requirements
  architecture.md          - System design
  setup_guide.md          - How to run locally
  api_documentation.md    - API endpoints (backend)
  testing_plan.md         - Test cases

o	nilego-mobile (Flutter Code)
o	nilego-firmware (ESP32 C++ Code)
•	Branching Strategy: main (Working code only) and dev (Active work)
 

In each repo 
# Create develop branch
git checkout -b develop
git push origin develop

# Set develop as default branch on GitHub
# (Go to Settings → Branches → Default branch)


**Branch strategy:**
- `main` - production/demo ready code
- `develop` - integration branch
- `feature/gps-integration` - individual features
- `bugfix/lock-timeout` - bug fixes

---

### **4. Add .gitignore Files**

**firmware/.gitignore:**

# Arduino
*.hex
*.bin
*.elf
build/


**mobile/.gitignore:**

# Flutter
.dart_tool/
.flutter-plugins
.packages
.pub-cache/
build/
*.g.dart

# Firebase config (IMPORTANT - don't commit keys)
google-services.json
firebase_options.dart
.env


**backend/.gitignore:**

# Firebase
.firebase/
node_modules/
.env
serviceAccountKey.json



### 5. Project Board Setup (Optional but useful)

On GitHub main page:

1. Go to "Projects" tab
2. Create new project: "NileGo Development"
3. Choose "Board" template
4. Add columns:
   - **Backlog**
   - **To Do**
   - **In Progress**
   - **Testing**
   - **Done**

5. Add initial tasks:
   - "Setup ESP32 development environment"
   - "Order hardware components"
   - "Create Firebase project"
   - "Design app UI mockups"
   - etc.

---

### **6. Issues/Tasks Template**

Create your first issues in each repo:

**Example Issue in firmware repo:**

Title: Setup ESP32 and test basic functionality

Description:
- [ ] Install Arduino IDE with ESP32 support
- [ ] Connect ESP32 to computer
- [ ] Upload Blink example
- [ ] Test Wi-Fi connection
- [ ] Test GPIO pin control

Assignee: [Your teammate]
Labels: setup, priority-high
Milestone: Week 1

