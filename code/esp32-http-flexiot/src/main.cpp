#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <SPI.h>
#include <DHT.h>

#define DHT_PIN 23
#define DHT_TYPE DHT11

const char *ssid = "ipop";
const char *password = "iyanfopi";
const char *mac = "7303568894610265";
char message[1024];
float temperature = 0, humidity = 0;
String tempString, humidString;

const size_t capacity = JSON_OBJECT_SIZE(5);
DynamicJsonDocument doc(capacity);

HTTPClient http;
DHT dht(DHT_PIN, DHT_TYPE);

void connectToNetwork();
void sendMessage(String payload);
void updateDhtData();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  dht.begin();

  connectToNetwork();
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (WiFi.status() == WL_CONNECTED) {
    updateDhtData();

    doc["eventName"] = "dht11Status";
    doc["status"] = "none";
    doc["temp"] = tempString.c_str();
    doc["humid"] = humidString.c_str();
    doc["mac"] = mac;

    serializeJson(doc, message);

    sendMessage(String(message));
  } else {
    connectToNetwork();
  }

  delay(10000);
}

void connectToNetwork() {
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Establishing connection to WiFi..");
  }

  Serial.println("Connected to network");
}

void sendMessage(String payload) {
  Serial.println(payload);
  http.begin("http://52.221.141.22:8080/api/pcs/Generic_brand_2003ESP32_TESThttp_test");
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(payload);

  if (httpResponseCode > 0) {
    String response = http.getString(); // Get the response to the request

    Serial.println(httpResponseCode); // Print return code
    Serial.println(response);         // Print request answer
  } else {
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

void updateDhtData() {
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  tempString = String(temperature);
  humidString = String(humidity);

  Serial.println("Temperature: " + tempString + " C & Humidity: " +
                 humidString + " %");
}