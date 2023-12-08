#include <Arduino.h>
#include <ArduinoJson.h>

const size_t capacity = JSON_OBJECT_SIZE(5) + 70;
DynamicJsonDocument doc(capacity);

const char *json =
    "{\"device_id\":1,\"status\":\"ON\",\"temp\":\"26.5\","
    "\"humid\":\"54.0\",\"mac\":\"204637284\"}";

void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);

    // Deserialize JSON
    deserializeJson(doc, json);

    int device_id = doc["device_id"];    // 1
    const char *status = doc["status"];  // "ON"
    const char *temp = doc["temp"];      // "26.5"
    const char *humid = doc["humid"];    // "54.0"
    const char *mac = doc["mac"];        // "204637284"

    ESP_LOGI("SETUP", "Device ID: %d", device_id);
    ESP_LOGI("SETUP", "Status: %s", status);
    ESP_LOGI("SETUP", "Suhu: %s", temp);
    ESP_LOGI("SETUP", "Kelembaban: %s", humid);
    ESP_LOGI("SETUP", "Mac: %s", mac);

    // Serialize JSON
    doc["device_id"] = 2;
    doc["status"] = "OFF";
    doc["temp"] = "23.5";
    doc["humid"] = "34.2";
    doc["mac"] = "84759327592";

    String serializeOutput;
    serializeJson(doc, serializeOutput);

    ESP_LOGI("SETUP", "Output JSON: %s", serializeOutput.c_str());
}

void loop() {
    // put your main code here, to run repeatedly:
}