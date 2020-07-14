#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

const char *ssid = "#########";
const char *password = "###########";
const size_t capacity = JSON_OBJECT_SIZE(2);
String message;

DynamicJsonDocument doc(capacity);
HTTPClient http;

void connectToNetwork();
void telePrintChatId();
void teleSendMessage(String payload);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  connectToNetwork();
  delay(1000);

  telePrintChatId();
  delay(3000);
  doc["chat_id"] = 153457830;
  doc["text"] = "Hello World";
  serializeJson(doc, message);
  teleSendMessage(message);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (WiFi.status() == WL_CONNECTED) {
    // doc["chat_id"] = 153457830;
    // doc["text"] = "Hello World";
    // serializeJson(doc, message);
    // teleSendMessage(message);
  } else {
    connectToNetwork();
  }

  delay(10000);

  // Deserialize JSON
}

void connectToNetwork() {
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Establishing connection to WiFi..");
  }

  Serial.println("Connected to network");
}

void telePrintChatId() {
  http.begin("https://api.telegram.org/"
             "bot1383965879:AAEII9ZEdPWYiAyH57JRseQVjMYvZmDBcKM/getUpdates");
  int httpResponseCode = http.GET();

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

void teleSendMessage(String payload) {
  Serial.println(payload);
  http.begin("https://api.telegram.org/"
             "bot1383965879:AAEII9ZEdPWYiAyH57JRseQVjMYvZmDBcKM/sendMessage");
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