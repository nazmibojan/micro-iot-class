#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <SPI.h>
#include <DHT.h>
#include "time.h"

#define DHT_PIN 23
#define DHT_TYPE DHT11

typedef struct sensorData
{
  /* data */
  struct tm timeinfo;
  float temperature;
  float humidity;
} sensorData;

const char *ssid = "******";
const char *password = "******";
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;
sensorData dht11Data;

DHT dht(DHT_PIN, DHT_TYPE);

void getLocalTime();
void updateDhtData();
void connectToNetwork();

void setup()
{
  Serial.begin(9600);
  dht.begin();
  
  connectToNetwork();
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop()
{
  getLocalTime();
  updateDhtData();
  Serial.print("Temperature: ");
  Serial.print(dht11Data.temperature);
  Serial.print(" C and Humidity: ");
  Serial.print(dht11Data.humidity);
  Serial.print(" % at ");
  Serial.println(&dht11Data.timeinfo, "%A, %B %d %Y %H:%M:%S");
  delay(5000);
}

void getLocalTime()
{
  if(!getLocalTime(&dht11Data.timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
}

void updateDhtData() {
  dht11Data.temperature = dht.readTemperature();
  dht11Data.humidity = dht.readHumidity();
}

void connectToNetwork() {
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Establishing connection to WiFi..");
  }

  Serial.println("Connected to network");
}