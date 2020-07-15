#include <Arduino.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <SPI.h>
#include <DHT.h>

// SIM800L Modem Pins
#define MODEM_RST 5
#define MODEM_TX 17
#define MODEM_RX 16

#define DHT_PIN 23
#define DHT_TYPE DHT11

#define PUBLISH_INTERVAL 10000

// Set serial for AT commands (to SIM800 module)
#define SerialAT Serial2

// Configure TinyGSM library
#define TINY_GSM_MODEM_SIM800 // Modem is SIM800

unsigned long lastPublish = 0;
float tempFloat, humidFloat;
String temperature;
String humidity;

DHT dht(DHT_PIN, DHT_TYPE);

String mqttServer = "	hairdresser.cloudmqtt.com";
String mqttUser = "nglettrq";
String mqttPwd = "RVPcR2AQJEV1";
String deviceId = "Home_Gateway_1";
String pubTopic = String(deviceId + "/sensor_data");
String subTopic = String(deviceId + "/led_status");
String mqttPort = "18848";

// Your GPRS credentials (leave empty, if not needed)
const char apn[] = "AXIS";        // APN
const char gprsUser[] = "axis";   // GPRS User
const char gprsPass[] = "123456"; // GPRS Password

#include <TinyGsmClient.h>

TinyGsm modem(SerialAT);
TinyGsmClient GsmClient(modem);
PubSubClient ESPMqtt(GsmClient);

void connectToGprs();
void connectToMqtt();
void publishMessage();
void updateDhtData();
void mqttCallback(char *topic, byte *payload, long length);
void do_actions(const char *message);

void setup() {
  Serial.begin(9600);
  delay(1000);
  pinMode(BUILTIN_LED, OUTPUT);
  dht.begin();

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

  // Disconnecting previous WiFi
  ESPMqtt.setServer(mqttServer.c_str(), mqttPort.toInt());
  ESPMqtt.setCallback(mqttCallback);

  connectToGprs();
  connectToMqtt();
}

void loop() {
  if (modem.isGprsConnected()) {
    Serial.println("Modem is offline. Reconnect modem...");
    connectToGprs();
  }

  if (modem.isGprsConnected() && !ESPMqtt.connected()) {
    connectToMqtt();
  }

  if (millis() - lastPublish > PUBLISH_INTERVAL) {
    updateDhtData();
    Serial.print("Get sensor Data: ");
    // Temp and Humidity
    Serial.print(" Temperature = ");
    Serial.print(tempFloat);
    Serial.print(" Humidity = ");
    Serial.print(humidFloat);
    publishMessage();

    lastPublish = millis();
  }

  ESPMqtt.loop();
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

void connectToMqtt() {
  while (!ESPMqtt.connected()) {
    Serial.println("ESP > Connecting to MQTT...");

    if (ESPMqtt.connect("ESP32Client", mqttUser.c_str(), mqttPwd.c_str())) {
      Serial.println("Connected to Server");
      // subscribe to the topic
      ESPMqtt.subscribe(subTopic.c_str());
    } else {
      Serial.print("ERROR > failed with state");
      Serial.print(ESPMqtt.state());
      Serial.print("\r\n");
      delay(2000);
    }
  }
}

void publishMessage() {
  char msgToSend[1024] = {0};
  const size_t capacity = JSON_OBJECT_SIZE(4);
  DynamicJsonDocument doc(capacity);

  temperature = String(tempFloat);
  humidity = String(humidFloat);

  doc["eventName"] = "sensorStatus";
  doc["status"] = "none";
  doc["temp"] = temperature.c_str();
  doc["humid"] = humidity.c_str();

  serializeJson(doc, msgToSend);

  // Serial.println(msgToSend);
  ESPMqtt.publish(pubTopic.c_str(), msgToSend);
}

void updateDhtData() {

  // TO DO: Data Verification
  // Get temperature and humidity data
  tempFloat = dht.readTemperature();
  humidFloat = dht.readHumidity();
}

void mqttCallback(char *topic, byte *payload, long length) {
  char msg[256];

  Serial.print("Message arrived [");
  Serial.print(subTopic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    msg[i] = (char)payload[i];
  }
  do_actions(msg);
}

void do_actions(const char *message) {
  const size_t capacity = JSON_OBJECT_SIZE(2) + 30;
  DynamicJsonDocument doc(capacity);

  deserializeJson(doc, message);

  const char *deviceId = doc["deviceId"];   
  const char *ledStatus = doc["ledStatus"]; 

  if (String(ledStatus) == "ON") {
    Serial.println("TURN ON LED");
    digitalWrite(BUILTIN_LED, HIGH);
  } else if (String(ledStatus) == "OFF") {
    Serial.println("TURN Off LED");
    digitalWrite(BUILTIN_LED, LOW);
  } else {
    Serial.println("Could not recognize LED Status");
  }
}