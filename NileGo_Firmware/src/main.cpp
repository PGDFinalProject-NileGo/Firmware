// #include <Arduino.h>
// #include <NimBLEDevice.h>

// // --- PINS ---
// #define LOCK_PIN      13   // The Solenoid Lock (MOSFET Gate)
// #define LED_PIN       2    // Onboard LED (Status)
// #define WAKE_BUTTON   33   // Button to wake up bike (Connect between Pin 33 and GND)

// // --- SETTINGS ---
// #define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
// #define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
// #define SLEEP_TIMEOUT_MS    60000  // Go to sleep if no connection after 60 seconds

// NimBLEServer *pServer = NULL;
// NimBLECharacteristic *pCharacteristic = NULL;
// bool deviceConnected = false;
// unsigned long lastActivityTime = 0;

// // Function to put device to sleep
// void goToDeepSleep() {
//   Serial.println("Timeout! Going to Deep Sleep...");
//   digitalWrite(LED_PIN, LOW);
//   digitalWrite(LOCK_PIN, LOW); // Ensure lock is safe
  
//   // Configure Wake Up: Wake up when Pin 33 is pulled LOW (Button Pressed)
//   esp_sleep_enable_ext0_wakeup((gpio_num_t)WAKE_BUTTON, 0); 
  
//   Serial.println("Goodnight.");
//   esp_deep_sleep_start();
// }

// // Lock Control
// void unlockBike() {
//   Serial.println("UNLOCKING...");
//   digitalWrite(LOCK_PIN, HIGH); // Open Lock
//   delay(3000);                  // Keep open 3 seconds
//   digitalWrite(LOCK_PIN, LOW);  // Close Lock
//   Serial.println("LOCKED.");
  
//   // Reset timer so it doesn't sleep immediately after unlocking
//   lastActivityTime = millis(); 
// }

// // Callbacks
// class MyServerCallbacks : public NimBLEServerCallbacks {
//   void onConnect(NimBLEServer* pServer) {
//     deviceConnected = true;
//     lastActivityTime = millis(); // Reset timer
//     Serial.println("Phone Connected!");
//   };

//   void onDisconnect(NimBLEServer* pServer) {
//     deviceConnected = false;
//     Serial.println("Phone Disconnected.");
//     NimBLEDevice::getAdvertising()->start(); 
//   }
// };

// class MyCallbacks : public NimBLECharacteristicCallbacks {
//   void onWrite(NimBLECharacteristic *pCharacteristic) {
//     std::string value = pCharacteristic->getValue();
//     if (value == "OPEN") {
//       unlockBike();
//     }
//   }
// };

// void setup() {
//   Serial.begin(115200);
  
//   // Print Wakeup Reason
//   if(esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0) {
//     Serial.println("Woke up by Button Press!");
//   } else {
//     Serial.println("Normal Boot.");
//   }

//   // Pin Setup
//   pinMode(LOCK_PIN, OUTPUT);
//   pinMode(LED_PIN, OUTPUT);
//   pinMode(WAKE_BUTTON, INPUT_PULLUP); // Important for the button!
  
//   digitalWrite(LOCK_PIN, LOW);
//   digitalWrite(LED_PIN, HIGH); // LED ON = Awake

//   // Bluetooth Setup
//   NimBLEDevice::init("NileGo_Bike_1");
//   pServer = NimBLEDevice::createServer();
//   pServer->setCallbacks(new MyServerCallbacks());
//   NimBLEService *pService = pServer->createService(SERVICE_UUID);
//   pCharacteristic = pService->createCharacteristic(
//                       CHARACTERISTIC_UUID,
//                       NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE_NR
//                     );
//   pCharacteristic->setCallbacks(new MyCallbacks());
//   pService->start();
//   NimBLEDevice::getAdvertising()->start();
  
//   lastActivityTime = millis();
// }

// void loop() {
//   // Check if it's time to sleep
//   if (millis() - lastActivityTime > SLEEP_TIMEOUT_MS) {
//     goToDeepSleep();
//   }
//   delay(100);
// }

#include <Arduino.h>
#include <NimBLEDevice.h>

// --- PINS (Matching your Wokwi) ---
#define LOCK_RELAY_PIN  2   // The Relay/Lock
#define SENSOR_PIN      14  // The Button/Sensor (Optional)
#define LED_PIN         2   // Blue Status LED (Onboard)

// --- BLUETOOTH SETTINGS ---
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

NimBLEServer *pServer = NULL;
NimBLECharacteristic *pCharacteristic = NULL;
bool deviceConnected = false;

// CALLBACKS
class MyServerCallbacks : public NimBLEServerCallbacks {
    void onConnect(NimBLEServer* pServer) {
      deviceConnected = true;
      Serial.println("📱 Phone Connected!");
    };

    void onDisconnect(NimBLEServer* pServer) {
      deviceConnected = false;
      Serial.println("📱 Phone Disconnected.");
      NimBLEDevice::startAdvertising(); // Keep looking for phones
    }
};

class MyCallbacks : public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();
      if (value.length() > 0) {
        Serial.print("Command Received: ");
        Serial.println(value.c_str());

        // LOGIC: IF APP SAYS "OPEN"
        if (value == "OPEN") {
          unlockBike();
        }
      }
    }
};

void unlockBike() {
  Serial.println("🔓 UNLOCKING BIKE...");
  digitalWrite(LOCK_RELAY_PIN, HIGH); // Turn Relay ON (Pop Lock)
  delay(3000);                        // Wait 3 Seconds
  digitalWrite(LOCK_RELAY_PIN, LOW);  // Turn Relay OFF
  Serial.println("🔒 LOCKED.");
}

void setup() {
  Serial.begin(115200);
  pinMode(LOCK_RELAY_PIN, OUTPUT);
  digitalWrite(LOCK_RELAY_PIN, LOW); // Start Locked

  // Initialize BLE
  NimBLEDevice::init("NileGo_Bike_1"); // Name shown in scanning
  pServer = NimBLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  
  NimBLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      NIMBLE_PROPERTY::READ | 
                      NIMBLE_PROPERTY::WRITE
                    );
  pCharacteristic->setCallbacks(new MyCallbacks());
  
  pService->start();
  NimBLEDevice::startAdvertising();
  Serial.println("✅ BLE Ready. Waiting for App...");
}

void loop() {
  // Nothing to do here, everything is handled in callbacks!
  delay(1000);
}