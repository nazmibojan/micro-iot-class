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
#define CHARACTERISTIC_UUID "5bc0180a-b3b8-11ea-b3de-0242ac130004"

DHT dht(DHT_PIN, DHT_TYPE);
float temperature = 0, humidity = 0;

BLECharacteristic *pCharacteristic;

void updateDhtData();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  dht.begin();

  BLEDevice::init("ESP32-BLE-Server");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ |
                               BLECharacteristic::PROPERTY_WRITE |
                               BLECharacteristic::PROPERTY_NOTIFY |
                               BLECharacteristic::PROPERTY_INDICATE);
  pCharacteristic->addDescriptor(new BLE2902());
  pService->start();

  pServer->getAdvertising()->start();
  Serial.println("Characteristic defined!");
}

void loop() {
  // put your main code here, to run repeatedly:
  updateDhtData();
  String sentence = "Suhu: " + String(temperature) + " C dan Kelembaban: " +
                    String(humidity) + " %";
  Serial.println(sentence);

  pCharacteristic->setValue(sentence.c_str());
  pCharacteristic->notify();
  delay(5000);
}

void updateDhtData() {
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
}