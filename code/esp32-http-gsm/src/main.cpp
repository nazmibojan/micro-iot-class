#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

// SIM800L Modem Pins
#define MODEM_RST 5
#define MODEM_TX 17
#define MODEM_RX 16

// Set serial for AT commands (to SIM800 module)
#define SerialAT Serial2

// Configure TinyGSM library
#define TINY_GSM_MODEM_SIM800 // Modem is SIM800

const char *ssid = "#########";
const char *password = "###########";
const size_t capacity = JSON_OBJECT_SIZE(2);
String message;

// Your GPRS credentials (leave empty, if not needed)
const char apn[] = "AXIS";        // APN
const char gprsUser[] = "axis";   // GPRS User
const char gprsPass[] = "123456"; // GPRS Password

#include <TinyGsmClient.h>

TinyGsm modem(SerialAT);
TinyGsmClient GsmClient(modem);
DynamicJsonDocument doc(capacity);
HTTPClient http;

void connectToGprs();
void telePrintChatId();
void teleSendMessage(String payload);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  // Initialize GSM Module
  pinMode(MODEM_RST, OUTPUT);
  digitalWrite(MODEM_RST, HIGH);
  SerialAT.begin(38400, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(6000);
  if (!modem.restart()) {
    Serial.println("Modem failed to restart");
  } else {
    Serial.println("Modem successfully restart");
  }
  delay(1000);

  connectToGprs();
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
  if (modem.isGprsConnected()) {
    // doc["chat_id"] = 153457830;
    // doc["text"] = "Hello World";
    // serializeJson(doc, message);
    // teleSendMessage(message);
  } else {
    connectToGprs();
  }

  delay(10000);
}

void connectToGprs() {
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    Serial.println("Failed to connect to GPRS network");
  } else {
    Serial.println("Successfully connected to GPRS network");
  }

  if (modem.isGprsConnected()) {
    Serial.println("Modem is online");
  } else {
    Serial.println("Modem is offline");
  }
}

void telePrintChatId() {
  http.begin("https://api.telegram.org/"
             "bot<BOT TOKEN>/getUpdates");
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
             "bot<BOT TOKEN>/sendMessage");
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