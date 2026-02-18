#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

// --- HARDWARE PIN DEFINITIONS ---

#define LOCK_PIN 13 // Connect to Relay IN
#define UNLOCK_TIME 5000 // 5 seconds
#define SENSOR_PIN 14     // Reed Switch (Connect between D14 and GND)

// --- BLUETOOTH UUIDS ---
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// --- GLOBAL VARIABLES ---
BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool unlockRequest = false;

// --- CALLBACKS ---
class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
  deviceConnected = true;
  Serial.println("Phone Connected!");
}
    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println("❌ Phone Disconnected. Reconnecting again...");
      pServer->getAdvertising()->start(); // Restart search for phones
    }


};

class MyCallbacks: public BLECharacteristicCallbacks {
void onWrite(BLECharacteristic *pCharacteristic) {
std::string value = pCharacteristic->getValue();
      if (value.length() > 0) {
        Serial.print("📥 Received Value: ");
        Serial.println(value.c_str());

        // Check if the phone sent "u" or "unlock"
        if (value == "OPEN") {
           unlockRequest = true; // Tell the loop to unlock!
        }
      }
    }
};

void setup() {
Serial.begin(115200);

// 1. Setup Lock Pin
pinMode(LOCK_PIN, OUTPUT);
digitalWrite(LOCK_PIN, LOW); // Start Locked

// 2. Setup Reed Switch (Critical for Feedback)
// IMPORTANT: This makes the Reed Switch work without a resistor
// LOW = Magnet is TOUCHING (Pin Retracted)
// HIGH = Magnet is GONE (Pin Extended)

pinMode(SENSOR_PIN, INPUT_PULLUP);
Serial.println("Starting BLE WORK!----------------------------");


// Initialize BLE
BLEDevice::init("NileGo_Bike_1");
pServer = BLEDevice::createServer();
pServer->setCallbacks(new MyServerCallbacks());

// //   // 2. Create the Service

BLEService *pService = pServer->createService(SERVICE_UUID);
// //   // 3. Create the Characteristic (The data channel)
pCharacteristic = pService->createCharacteristic(
CHARACTERISTIC_UUID,
BLECharacteristic::PROPERTY_READ |
BLECharacteristic::PROPERTY_WRITE |
BLECharacteristic::PROPERTY_NOTIFY
);
// Add the Descriptor (Required for Notify to work)
pCharacteristic->addDescriptor(new BLE2902()); 

pCharacteristic->setCallbacks(new MyCallbacks());
// //   // 4. Start the Service

pService->start();

// //   // 5. Start Advertising (So phones can find it)

// 2. Create the Advertising Data object
BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();

// 3. EXPLICITLY set the name in the advertisement
BLEAdvertisementData oAdvertisementData;
oAdvertisementData.setName("NileGo_Bike_1");
oAdvertisementData.setFlags(0x04); // BR_EDR_NOT_SUPPORTED 
pAdvertising->setAdvertisementData(oAdvertisementData);

// 4. Add the service and scan response as you had it
pAdvertising->addServiceUUID(SERVICE_UUID);
pAdvertising->setScanResponse(true);
pAdvertising->setMinPreferred(0x06);  
pAdvertising->setMinPreferred(0x12);
BLEDevice::startAdvertising();

Serial.println("READY! Waiting for Flutter App...");
}

void loop() {
  // //   // We handle the unlocking here to avoid blocking the Bluetooth connection

if (unlockRequest) {
Serial.println("--- UNLOCK COMMAND RECEIVED ---");
// 1. Fire the Relay (Unlock)
digitalWrite(LOCK_PIN, HIGH); // Unlock the bike


    // Wait up to 2 seconds for the reed switch to confirm open
    // Reed switch with INPUT_PULLUP reads LOW when closed (magnet present)
    // and HIGH when open (magnet removed = lock physically open)
    // bool confirmed = false;
bool confirmedOpen = false;
for (int i = 0; i < 100; i++) {
    // LOW means Magnet Detected (Pin Retracted/Open)
    // CORRECT: HIGH means the magnet is gone.
// The pin has successfully pushed out to the "Locked" position.

    if (digitalRead(SENSOR_PIN) == LOW) { 
        confirmedOpen = true;
        break; // We found it! Stop waiting.
    }
    delay(10);
}
if (confirmedOpen) {
   Serial.println("✅ SUCCESS: Sensor confirms lock is OPEN.");
   // Send message to Flutter App
   pCharacteristic->setValue("UNLOCKED");
   pCharacteristic->notify();
} else {
   Serial.println("⚠️ WARNING: Relay fired, but Sensor sees CLOSED.");
   pCharacteristic->setValue("ERROR_JAMMED");
   pCharacteristic->notify();// Send status to phone
}

// 3. Keep unlocked for 5 seconds
    delay(UNLOCK_TIME);
    // 4. Lock it again
    digitalWrite(LOCK_PIN, LOW);

// 5. CHECK LOCK STATUS
// Wait up to 1 second for the magnet to LEAVE (HIGH)
bool confirmedClosed = false;
for (int i = 0; i < 100; i++) {
    // HIGH means Magnet is GONE (Pin pushed out)
    if (digitalRead(SENSOR_PIN) == HIGH) { 
        confirmedClosed = true;
        break; 
    }
    delay(10);
}

if (confirmedClosed) {
    Serial.println("🔒 System Relocked.");
    pCharacteristic->setValue("LOCKED");
} else {
    Serial.println("⚠️ WARNING: System tried to lock, but sensor still sees magnet!");
    pCharacteristic->setValue("LOCK_FAIL");
}
pCharacteristic->notify();

unlockRequest = false; 
}


delay(20);
} // closes loop()