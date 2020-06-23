#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <DHT.h>

#define DHT_PIN 5
#define DHT_TYPE DHT11

DHT dht(DHT_PIN, DHT_TYPE);
float temperature = 0;

void readTemp();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  readTemp();
  Serial.println(temperature);
  delay(5000);
}

void readTemp() {
  temperature = dht.readTemperature();
}