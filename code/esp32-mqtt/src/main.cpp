#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <SPI.h>
#include <DHT.h>
#include <HTTPUpdate.h>

#define DHT_PIN 5
#define DHT_TYPE DHT11

#define PUBLISH_INTERVAL 10000

unsigned long lastPublish = 0;
float tempFloat, humidFloat;
String temperature;
String humidity;

DHT dht(DHT_PIN, DHT_TYPE);

String ssid = "Nuclear Physics Lab";
String pass = "einstein";
String mqttServer = "broker.hivemq.com";
String mqttUser = "";
String mqttPwd = "";
String deviceId = "Home_Gateway_1";
String pubTopic = String(deviceId + "/sensor_data");
String subTopic = String(deviceId + "/led_status");
String mqttPort = "1883";

WiFiClient ESPClient;
PubSubClient ESPMqtt(ESPClient);

void connectToNetwork();
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

  // Disconnecting previous WiFi
  WiFi.disconnect();
  ESPMqtt.setServer(mqttServer.c_str(), mqttPort.toInt());
  ESPMqtt.setCallback(mqttCallback);

  connectToNetwork();
  connectToMqtt();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectToNetwork();
  }

  if (WiFi.status() == WL_CONNECTED && !ESPMqtt.connected()) {
    connectToMqtt();
  }

  if (millis() - lastPublish > PUBLISH_INTERVAL) {
    updateDhtData();
    Serial.print("Get sensor Data: ");
    // Temp and Humidity
    Serial.print(" Temperature = ");
    Serial.print(tempFloat);
    Serial.print(" Humidity = ");
    Serial.println(humidFloat);
    publishMessage();

    lastPublish = millis();
  }

  ESPMqtt.loop();
}

void connectToNetwork() {
  delay(10);
  WiFi.begin(ssid.c_str(), pass.c_str());

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
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

  Serial.println(msgToSend);
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