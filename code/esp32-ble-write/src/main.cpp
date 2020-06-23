#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#define SERVICE_UUID "4d933c08-b3b8-11ea-b3de-0242ac130004"
#define CHARACTERISTIC_UUID "5bc0180a-b3b8-11ea-b3de-0242ac130004"

BLECharacteristic *pCharacteristic;

class BLECallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string rxValue = pCharacteristic->getValue();
    // Serial.println(rxValue[0]);

    if (rxValue.length() > 0) {
      Serial.println("*********");
      Serial.print("Received String: ");

      for (int i = 0; i < rxValue.length(); i++) {
        Serial.print(rxValue[i]);
      }
      Serial.println();
      Serial.println("*********");
    }

    if (rxValue == "led on") {
      Serial.println("Turning ON LED!");
      digitalWrite(BUILTIN_LED, HIGH);
    } else if (rxValue == "led off") {
      Serial.println("Turning OFF LED!");
      digitalWrite(BUILTIN_LED, LOW);
    } else {
      Serial.println("Could not recognize command");
    }
  }
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(BUILTIN_LED, OUTPUT);

  BLEDevice::init("ESP32-BLE-Server");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ |
                               BLECharacteristic::PROPERTY_WRITE |
                               BLECharacteristic::PROPERTY_NOTIFY |
                               BLECharacteristic::PROPERTY_INDICATE);
  pCharacteristic->addDescriptor(new BLE2902());
  pCharacteristic->setValue("Write command here...");
  pCharacteristic->setCallbacks(new BLECallback());
  pService->start();

  pServer->getAdvertising()->start();
  Serial.println("Characteristic defined!");
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(5000);
}