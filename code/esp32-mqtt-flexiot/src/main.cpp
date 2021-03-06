#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <SPI.h>
#include <DHT.h>

#define DHT_PIN 23
#define DHT_TYPE DHT11

#define PUBLISH_INTERVAL 10000

unsigned long lastPublish = 0;
float tempFloat, humidFloat;
String temperature;
String humidity;

DHT dht(DHT_PIN, DHT_TYPE);

String ssid = "Mako Brimob";
String pass = "mantapjiwa";
String mqttServer = "mqtt.flexiot.xl.co.id";
String mqttUser = "generic_brand_2003-esp32_test-mqtt_test_4814";
String mqttPwd = "1595751290_4814";
String deviceId = "Home_Gateway_1";
String mac = "3143861076070942";
String pubTopic = "generic_brand_2003/esp32_test/mqtt_test/common";
String subTopic = String("+/" + mac + "/generic_brand_2003/esp32_test/mqtt_test/sub");
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
  Serial.println("Try connecting to WiFi");
  WiFi.begin(ssid.c_str(), pass.c_str());

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void connectToMqtt() {
  while (!ESPMqtt.connected()) {
    Serial.println("ESP > Connecting to MQTT...");

    if (ESPMqtt.connect(deviceId.c_str(), mqttUser.c_str(), mqttPwd.c_str())) {
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
  const size_t capacity = JSON_OBJECT_SIZE(5);
  DynamicJsonDocument doc(capacity);

  temperature = String(tempFloat);
  humidity = String(humidFloat);

  doc["eventName"] = "dht11Status";
  doc["status"] = "none";
  doc["temp"] = temperature.c_str();
  doc["humid"] = humidity.c_str();
  doc["mac"] = mac.c_str();

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
  const size_t capacity = 2*JSON_OBJECT_SIZE(2) + 60;
  DynamicJsonDocument doc(capacity);

  deserializeJson(doc, message);

  const char* action = doc["action"];
  const char* param_mac = doc["param"]["mac"]; 
  const char* param_value = doc["param"]["value"];

  if (String(param_value) == "on") {
    Serial.println("TURN ON LED");
    digitalWrite(BUILTIN_LED, HIGH);
  } else if (String(param_value) == "off") {
    Serial.println("TURN Off LED");
    digitalWrite(BUILTIN_LED, LOW);
  } else {
    Serial.println("Could not recognize LED Status");
  }
}

