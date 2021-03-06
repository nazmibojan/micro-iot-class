#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <SPI.h>
#include <Preferences.h>
#include <time.h>
#include <HTTPUpdate.h>
#include <SoftwareSerial.h>

#define FIRST_FIRMWARE_FLASH

#define TRIGGER1_PIN 23
#define ECHO1_PIN 22
#define LED_PIN 2

#define PUBLISH_INTERVAL 20000
#define NTP_UPDATE_INTERVAL 60000

typedef struct sensorData {
  /* data */
  struct tm timeinfo;
  float rangeInCm;
} sensorData;

typedef struct firmwareData {
  String paramUrl;
  String paramBinName;
  String paramVersion;
} firmwareData;

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 25200;
const int daylightOffset_sec = 0;
unsigned long lastPublish = 0, lastNTPUpdate = 0;
// Variables for range measurement filter
float yimin1;
float alpha = 0.6;
float rangeData;

String ssid = "Nuclear Physics Lab";
String pass = "einstein22";
String mqttServer = "mqtt.flexiot.xl.co.id";
String mqttPort = "1883";
String mqttUser = "generic_brand_766-waterlevel_device-wlv_v2_5970";
String mqttPwd = "1609654426_5970";
String pubTopic = "generic_brand_766/waterlevel_device/wlv_v2/common";
String subTopic, deviceId, mac;
String fwVersion;

WiFiClient ESPClient;
PubSubClient ESPMqtt(ESPClient);
Preferences deviceData;
sensorData rawData;
firmwareData fwData;
SoftwareSerial mySerial(ECHO1_PIN, TRIGGER1_PIN);

void ntpUpdate();
void wifiSetupConnection();
void mqttSetupConnection();
void mqttPublishMessage();
void mqttCallback(char *topic, byte *payload, long length);
void rangeSensorUpdate();
bool otaCompareVersion(firmwareData *data);
void otaUpdate(String url, String bin, String version);

void setup() {
  Serial.begin(9600);
  pinMode(TRIGGER1_PIN, OUTPUT);
  pinMode(ECHO1_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  mySerial.begin(9600);
  delay(1000);

#ifdef FIRST_FIRMWARE_FLASH
  deviceData.begin("DeviceData", false);
  deviceData.putString("mac", "sensor0001");
  deviceData.putString("deviceid", "Sensor-1");
  deviceData.putString("fwversion", "1.0.0");
  deviceData.end();
#endif

  deviceData.begin("DeviceData", false);
  mac = deviceData.getString("mac");
  subTopic =
      String("+/" + mac + "/generic_brand_766/waterlevel_device/wlv_v2/sub");
  deviceId = deviceData.getString("deviceid");
  fwVersion = deviceData.getString("fwversion");
  deviceData.end();

  // Disconnecting previous WiFi
  WiFi.disconnect();
  ESPMqtt.setServer(mqttServer.c_str(), mqttPort.toInt());
  ESPMqtt.setCallback(mqttCallback);

  wifiSetupConnection();
  mqttSetupConnection();
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    wifiSetupConnection();
  }

  if (WiFi.status() == WL_CONNECTED && !ESPMqtt.connected()) {
    mqttSetupConnection();
  }

  if (millis() - lastPublish > PUBLISH_INTERVAL) {
    rangeSensorUpdate();
    mqttPublishMessage();

    lastPublish = millis();
  }

  if (millis() - lastNTPUpdate > NTP_UPDATE_INTERVAL) {
    ntpUpdate();

    // Get current time
    int hourNow = rawData.timeinfo.tm_hour;
    int minNow = rawData.timeinfo.tm_min;
    int secNow = rawData.timeinfo.tm_sec;
    Serial.println("[NTP] Current Time: " + String(hourNow) + ":" +
                   String(minNow) + ":" + String(secNow));

    // Check firmware version
    if (hourNow == 20 && minNow == 35) {
      Serial.println("[OTA] Check newest firmware");
      if (otaCompareVersion(&fwData)) {
        // Execute OTA Update
        Serial.println("[OTA] Start OTA Update");
        otaUpdate(String(fwData.paramUrl), String(fwData.paramBinName), String(fwData.paramVersion));
      }
    }

    lastNTPUpdate = millis();
  }

  ESPMqtt.loop();
}

void ntpUpdate() {
  if (!getLocalTime(&rawData.timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
}

void wifiSetupConnection() {
  delay(10);
  Serial.println("[WIFI] Try connecting to WiFi: " + ssid);
  WiFi.begin(ssid.c_str(), pass.c_str());

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println("[WIFI] Successfully connect to WiFi");
}

void mqttSetupConnection() {
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

void mqttPublishMessage() {
  char msgToSend[1024] = {0};
  DynamicJsonDocument doc(128);
  String rangeString = String(rangeData);

  doc["eventName"] = "sensorData";
  doc["status"] = "none";
  doc["range"] = rangeString.c_str();
  doc["mac"] = mac.c_str();

  serializeJson(doc, msgToSend);
  Serial.println("[MQTT] JSON data to publish: " + String(msgToSend));
  ESPMqtt.publish(pubTopic.c_str(), msgToSend);
}

void mqttCallback(char *topic, byte *payload, long length) {
  char msg[256];

  Serial.print("Message arrived [");
  Serial.print(subTopic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    msg[i] = (char)payload[i];
  }
}

void rangeSensorUpdate() {
  uint8_t pos = 0;
  uint8_t buffer[5];

  // Request data from sensor
  Serial.print("[SENSOR] Start requesting sensor data. Received: ");
  mySerial.write(0x55);
  delay(50);

  while (mySerial.available()) {
    delay(50);
    char inByte = mySerial.read();
    Serial.print(inByte, HEX);
    Serial.print(" ");
    if (inByte == 0xFF && pos == 0) {
      buffer[pos] = inByte;
      pos++;
    } else {
      buffer[pos] = inByte;
      if (pos == 3) {
        // frame complete, check sum
        uint8_t sumCheck = 0;
        for (int p = 0; p < 3; p++) {
          sumCheck += buffer[p];
        }
        // check checksum validity
        if ((uint8_t)buffer[3] == sumCheck) {
          // yey, packet valid
          uint16_t res;
          res = (uint16_t)buffer[1] << 8 | (uint16_t)buffer[2];
          pos = 0;

          // Filter noise
          float yi = yimin1 + alpha * ((float)res - yimin1);
          yimin1 = yi;
          rangeData = (yi / 10);

          if (res != 0) {
            Serial.println("[SENSOR] Distance: " + String(rangeData) + " cm");
            if (yi / 10.0 < 450.0 &&
                (yi / 10.0 > 25.0)) // minimum reading range
            {
              Serial.println("[SENSOR] Sensor range is valid");
            } else {
              Serial.println("[SENSOR] Sensor range is not valid");
            }
          } else {
            Serial.println("[SENSOR] Couldn't obtain data from sensor");
          }
        } else {
          // invalid packet
          Serial.println("[SENSOR] Invalid sensor data: checksum error");
          pos = 0;
        }
      } else {
        pos++;
      }
    }
  }
  Serial.println("[SENSOR] Done reading sensor data");
}

bool otaCompareVersion(firmwareData *data) {
  char *otaMajMinPatch[3];
  char *currMajMinPatch[3];
  char *otaPtr = NULL;
  char *currPtr = NULL;
  uint8_t versionIndex = 0;
  long otaMajor, otaMinor, otaPatch;
  long currMajor, currMinor, currPatch;
  String currVersion = fwVersion;
  HTTPClient http;

  http.begin("http://192.168.43.210:5000/getVersion");
  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    String response = http.getString(); // Get the response to the request
    Serial.println("[OTA] Return Code: " +
                   httpResponseCode); // Print return code
    Serial.println("[OTA] Get version response: " +
                   response); // Print request answer

    // Parsing JSON Response
    DynamicJsonDocument doc(192);
    deserializeJson(doc, response.c_str());
    const char* url = doc["url"];
    const char* filename = doc["filename"];
    const char* otaVersion = doc["version"];
    const char* date = doc["date"];

    // Copy OTA details to firmware data
    data->paramUrl = String(url);
    data->paramBinName = String(filename);
    data->paramVersion = String(otaVersion);

    // Parsing ota version
    otaPtr = strtok((char *)otaVersion, ".");
    while (otaPtr != NULL) {
      otaMajMinPatch[versionIndex] = otaPtr;
      versionIndex++;
      otaPtr = strtok(NULL, "."); // takes a list of delimiters
    }
  } else {
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
  }
  http.end();

  // Parsing current version
  currPtr = strtok((char *)currVersion.c_str(), ".");
  while (currPtr != NULL) {
    currMajMinPatch[versionIndex] = currPtr;
    versionIndex++;
    currPtr = strtok(NULL, "."); // takes a list of delimiters
  }
  versionIndex = 0;

  // Translate version to long int
  otaMajor = String(otaMajMinPatch[0]).toInt();
  otaMinor = String(otaMajMinPatch[1]).toInt();
  otaPatch = String(otaMajMinPatch[2]).toInt();
  currMajor = String(currMajMinPatch[0]).toInt();
  currMinor = String(currMajMinPatch[1]).toInt();
  currPatch = String(currMajMinPatch[2]).toInt();

  Serial.println("[OTA] OTA Major Version: " + String(otaMajor));
  Serial.println("[OTA] OTA Minor Version: " + String(otaMinor));
  Serial.println("[OTA] OTA Patch Version: " + String(otaPatch));
  Serial.println("[OTA] Current Major Version: " + String(currMajor));
  Serial.println("[OTA] Current Minor Version: " + String(currMinor));
  Serial.println("[OTA] Current Patch Version: " + String(currPatch));

  // Compare version
  if (otaMajor > currMajor) {
    return true;
  } else if (otaMajor < currMajor) {
    return false;
  } else {
    if (otaMinor > currMinor) {
      return true;
    } else if (otaMinor < currMinor) {
      return false;
    } else {
      if (otaPatch > currPatch) {
        return true;
      } else {
        return false;
      }
    }
  }
}

void otaUpdate(String url, String bin, String version) {
  WiFiClient otaClient;

  httpUpdate.rebootOnUpdate(false);
  Serial.println("[OTA] Connecting to: " + url + " for downloading " + bin +
                 " as new firmware with version " + version);
  t_httpUpdate_return ret = httpUpdate.update(otaClient, url, 5000, bin);

  switch (ret) {
  case HTTP_UPDATE_FAILED:
    Serial.printf("[NULL] (OTA) HTTP_UPDATE_FAILED Error (%d): %s\n",
                  httpUpdate.getLastError(),
                  httpUpdate.getLastErrorString().c_str());
    break;

  case HTTP_UPDATE_NO_UPDATES:
    Serial.println("[OTA] HTTP_UPDATE_NO_UPDATES");
    break;

  case HTTP_UPDATE_OK:
    Serial.println("[OTA] HTTP_UPDATE_OK. Write version to EEPROM. Reboot "
                   "gateway in 30s.");
    deviceData.begin("DeviceData", false);
    deviceData.putString("fwversion", version);
    deviceData.end();
    delay(30000);
    ESP.restart();
    break;
  }
}