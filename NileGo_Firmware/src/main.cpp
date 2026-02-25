#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

// --- HARDWARE PIN DEFINITIONS ---
#define LOCK_PIN        13 // Relay (Active HIGH)
#define SENSOR_PIN      14 // Reed Switch 
#define LED_PIN         15 // Status LED
//#define BUZZER_PIN      12 // Buzzer
#define UNLOCK_TIME 5000    // 5 Seconds 


// --- BLUETOOTH UUIDS (MATCHING YOUR FLUTTER APP) ---
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"


// --- GLOBAL VARIABLES ---
BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

// Timer Logic (Non-blocking)
unsigned long unlockTimestamp = 0;  // Tracks when we started unlocking
bool isUnlocked = false;            // Tracks if the solenoid is currently active

// State Tracking
bool lastLockedState = false;       // To detect changes in the Reed Switch

// --- CALLBACKS ---
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      Serial.println("📱 Phone Connected!");
      digitalWrite(LED_PIN, HIGH); // Visual feedback
      
      // Connection Beep (Short Blip)
      //digitalWrite(BUZZER_PIN, HIGH); delay(100); digitalWrite(BUZZER_PIN, LOW);
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println("❌ Phone Disconnected.");
      digitalWrite(LED_PIN, LOW);
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();

      if (value.length() > 0) {
        Serial.print("📥 Received: ");
        Serial.println(value.c_str());

        // Handle "OPEN" (From your Flutter App) or "UNLOCK"
        if (value == "OPEN" || value == "UNLOCK") { 
           Serial.println("Action: Unlocking...");
           
           // 1. Activate Hardware
           digitalWrite(LOCK_PIN, HIGH); // Fire Relay
           isUnlocked = true;
           unlockTimestamp = millis();   // Start the timer NOW
           
           // 2. Feedback (Long Beep)
           //digitalWrite(BUZZER_PIN, HIGH); delay(200); digitalWrite(BUZZER_PIN, LOW);
           
           // 3. Notify App immediately
           pCharacteristic->setValue("UNLOCKED");
           pCharacteristic->notify();
        }
      }
    }
};

void setup() {
  Serial.begin(115200);

  // 1. Setup Pins
  pinMode(LOCK_PIN, OUTPUT);
  //pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(SENSOR_PIN, INPUT_PULLUP); // LOW = Magnet Detected (Locked)

  // 2. Initial State
  digitalWrite(LOCK_PIN, LOW); // Start Locked
  digitalWrite(LED_PIN, LOW);
  //digitalWrite(BUZZER_PIN, LOW);

  // 3. Initialize BLE (Using your ORIGINAL Names/UUIDs)
  BLEDevice::init("NileGo_Bike_1");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create Single Characteristic for Read/Write/Notify (Old Style)
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );

  pCharacteristic->addDescriptor(new BLE2902());
  pCharacteristic->setCallbacks(new MyCallbacks());

  pService->start();

  // 4. Advertising (Preserved from your code)
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  
  Serial.println("✅ System Ready (Flutter Compatible Mode)");
}

void loop() {
  // --- TASK 1: Handle Auto-Lock Timer (Non-blocking) ---
  if (isUnlocked) {
    // Check if UNLOCK_TIME (5000ms) has passed since unlockTimestamp
    if (millis() - unlockTimestamp > UNLOCK_TIME) {
        digitalWrite(LOCK_PIN, LOW); // Cut power to solenoid
        isUnlocked = false;
        Serial.println("⏳ Timeout: Power cut to Solenoid.");
    }
  }

  // --- TASK 2: Monitor Reed Switch (Sensor) ---
  // Logic: "Locked" means Magnet IS detected (LOW) AND Solenoid is OFF (isUnlocked == false)
  bool isMagnetDetected = (digitalRead(SENSOR_PIN) == LOW);
  bool currentSystemState = isMagnetDetected && !isUnlocked;

  // Only send updates if the physical state actually changes
  if (currentSystemState != lastLockedState) {
      lastLockedState = currentSystemState;
      
      if (deviceConnected) {
          if (currentSystemState) {
              // Bike is physically locked
              Serial.println("🔒 Sensor Confirmed: LOCKED");
              pCharacteristic->setValue("LOCKED");
              
              // Lock Confirmation Beep (Double Beep)
              //digitalWrite(BUZZER_PIN, HIGH); delay(100); digitalWrite(BUZZER_PIN, LOW);
              //delay(100);
             // digitalWrite(BUZZER_PIN, HIGH); delay(100); digitalWrite(BUZZER_PIN, LOW);
              
          } else {
              // Bike is physically open
              Serial.println("🔓 Sensor Confirmed: OPEN");
              // We don't necessarily send "OPEN" unless App needs it, 
              // but we definitely update the internal state.
          }
          pCharacteristic->notify();
      }
  }

  // --- TASK 3: Manage Reconnection ---
  if (!deviceConnected && oldDeviceConnected) {
      delay(500); 
      pServer->startAdvertising(); 
      Serial.println("📡 Advertising restarted...");
      oldDeviceConnected = deviceConnected;
  }
  if (deviceConnected && !oldDeviceConnected) {
      oldDeviceConnected = deviceConnected;
  }
  
  delay(50); // Small delay for stability
}