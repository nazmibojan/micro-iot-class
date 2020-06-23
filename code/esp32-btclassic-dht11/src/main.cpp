#include <Arduino.h>
#include <BluetoothSerial.h>
#include <Wire.h>
#include <SPI.h>
#include <DHT.h>

#define DHT_PIN 5
#define DHT_TYPE DHT11

BluetoothSerial SerialBT;
DHT dht(DHT_PIN, DHT_TYPE);
float temperature = 0, humidity = 0;

void updateDhtData();

void setup() {
  // put your setup code here, to run once:
  SerialBT.begin("ESP32-Test");
  dht.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  updateDhtData();
  String sentence = "Data sensor sekarang -> Suhu: " + String(temperature) + \
                    " C dan Kelembaban: " + String(humidity) + " %";
  SerialBT.println(sentence);
  delay(5000);
}

void updateDhtData() {
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
}