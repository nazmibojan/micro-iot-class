#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <Wire.h>
#include <SPI.h>
#include <DHT.h>

#define DHT_PIN 5
#define DHT_TYPE DHT11

#define SERVICE_UUID "4d933c08-b3b8-11ea-b3de-0242ac130004"
#define TX_CHARACTERISTIC_UUID "5bc0180a-b3b8-11ea-b3de-0242ac130004"
#define RX_CHARACTERISTIC_UUID "a0796de5-1520-459b-acc7-da745ff7b434"

DHT dht(DHT_PIN, DHT_TYPE);
float temperature = 0, humidity = 0;

BLECharacteristic *pTxCharacteristic, *pRxCharacteristic;

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

    if (rxValue == "1") {
      Serial.println("Turning ON LED!");
      digitalWrite(BUILTIN_LED, HIGH);
    } else if (rxValue == "0") {
      Serial.println("Turning OFF LED!");
      digitalWrite(BUILTIN_LED, LOW);
    } else {
      Serial.println("Could not recognize command");
    }
  }
};

void updateDhtData();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(BUILTIN_LED, OUTPUT);
  dht.begin();

  BLEDevice::init("ESP32-BLE-TxRx");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  // Sensor Data Characteristic
  pTxCharacteristic = pService->createCharacteristic(
      TX_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ |
                                  BLECharacteristic::PROPERTY_NOTIFY |
                                  BLECharacteristic::PROPERTY_INDICATE);
  pTxCharacteristic->addDescriptor(new BLE2902());
  // Receive command characteristic
  pRxCharacteristic = pService->createCharacteristic(
      RX_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ |
                                  BLECharacteristic::PROPERTY_WRITE |
                                  BLECharacteristic::PROPERTY_NOTIFY |
                                  BLECharacteristic::PROPERTY_INDICATE);
  pRxCharacteristic->addDescriptor(new BLE2902());
  pRxCharacteristic->setValue("0");
  pRxCharacteristic->setCallbacks(new BLECallback());
  pService->start();

  pServer->getAdvertising()->setMinPreferred(0x06);
  pServer->getAdvertising()->setMaxPreferred(0x12);
  pServer->getAdvertising()->start();
  Serial.println("Characteristic defined!");
}

void loop() {
  // put your main code here, to run repeatedly:
  updateDhtData();
  String sensorData = String(temperature) + ";" + String(humidity);
  Serial.println(sensorData);

  pTxCharacteristic->setValue(sensorData.c_str());
  pTxCharacteristic->notify();
  delay(5000);
}

void updateDhtData() {
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
}