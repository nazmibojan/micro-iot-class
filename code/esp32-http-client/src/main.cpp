#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

const char *ssid = "Praktisi AP";
const char *password = "semangka";
const size_t capacity = JSON_OBJECT_SIZE(2);
char message[1024];

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

  // doc["chat_id"] = 153457830;
  // doc["text"] = "Hello World!";
  // serializeJson(doc, message);
  // teleSendMessage(message);
}

void loop() {
}

void connectToNetwork() {
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    ESP_LOGI("WIFI", "Establishing connection to WiFi..");
  }

  ESP_LOGI("WIFI", "Connected to network");
}

void telePrintChatId() {
  http.begin("https://api.telegram.org/bot1383965879:AAEII9ZEdPWYiAyH57JRseQVjMYvZmDBcKM/getUpdates");
  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    String response = http.getString(); // Get the response to the request

    ESP_LOGI("TELE", "HTTP Response code: %d", httpResponseCode); // Print return code
    ESP_LOGI("TELE", "Response: %s", response.c_str());         // Print request answer
  } else {
    ESP_LOGI("TELE", "Error on sending POST: %d", httpResponseCode);
  }

  http.end();
}

void teleSendMessage(String payload) {
  ESP_LOGI("TELE", "HTTP Payload: %s", payload.c_str());
  http.begin("https://api.telegram.org/"
             "bot1383965879:AAEII9ZEdPWYiAyH57JRseQVjMYvZmDBcKM/sendMessage");
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(payload);

  if (httpResponseCode > 0) {
    String response = http.getString(); // Get the response to the request

    ESP_LOGI("TELE", "HTTP Response code: %d", httpResponseCode); // Print return code
    ESP_LOGI("TELE", "Response: %s", response.c_str());         // Print request answer
  } else {
    ESP_LOGI("TELE", "Error on sending POST: %d", httpResponseCode);
  }

  http.end();
}