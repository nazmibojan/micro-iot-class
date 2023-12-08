#include <Arduino.h>
#include <ArduinoJson.h>
#include <BH1750.h>
#include <DHT.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <Wire.h>

#define DHT_PIN 4
#define DHT_TYPE DHT11

#define PUBLISH_INTERVAL 1000

const char *ssid = "NZM IoT Lab";
const char *pass = "Heisenberg1932";
int lastRequest = 0;
float tempFloat, humidFloat;
float lux;

TaskHandle_t dht11Handle;
TaskHandle_t lightHandle;
TaskHandle_t mqttTaskHandle;

SemaphoreHandle_t xCountingSemaphore;

DHT dht(DHT_PIN, DHT_TYPE);
BH1750 lightMeter;

String mqttServer = "broker.hivemq.com";
String mqttUser = "";
String mqttPwd = "";
String deviceId = "Home_Gateway_1";
String pubTopic = String(deviceId + "/sensor_data");
String mqttPort = "1883";

WiFiClient ESPClient;
PubSubClient ESPMqtt(ESPClient);

void dht11Task(void *parameter);
void lightTask(void *parameter);
void mqttTask(void *parameter);
boolean updateDhtData();
boolean updateLightData();
void connectToMqtt();
void connectToNetwork();
void publishMessage();

void setup() {
    // put your setup code here, to run once:
    pinMode(BUILTIN_LED, OUTPUT);
    Serial.begin(9600);
    vTaskDelay(1000);

    connectToNetwork();

    xCountingSemaphore = xSemaphoreCreateCounting(2, 2);

    // Create RTOS task
    xTaskCreate(dht11Task, "DHT11 Task", 2048, NULL, 1, &dht11Handle);
    xTaskCreate(lightTask, "Light Task", 2048, NULL, 1, &lightHandle);
    xTaskCreate(mqttTask, "MQTT Task", 4096, NULL, 2, &mqttTaskHandle);
}

void loop() {
}

void dht11Task(void *parameter) {

    for (;;) {
        xSemaphoreTake(xCountingSemaphore, portMAX_DELAY);
        while (!updateDhtData()) {
            ESP_LOGI("DHT11", "dht11 error! Retry in 3 seconds");
            vTaskDelay(3000);
        }
        ESP_LOGI("DHT11", "Temperature = %.2f C & Humidity = %.2f %", tempFloat, humidFloat);
        xSemaphoreGive(xCountingSemaphore);

        vTaskDelay(PUBLISH_INTERVAL);
    }
}

void lightTask(void *parameter) {

    Wire.begin();
    // On esp8266 you can select SCL and SDA pins using Wire.begin(D4, D3);
    // For Wemos / Lolin D1 Mini Pro and the Ambient Light shield use Wire.begin(D2, D1);

    lightMeter.begin();

    for (;;) {
        xSemaphoreTake(xCountingSemaphore, portMAX_DELAY);
        while (!updateLightData()) {
            ESP_LOGI("LIGHT", "BH1750FVI error! Retry in 3 seconds");
            vTaskDelay(3000);
        }
        ESP_LOGI("LIGHT", "Light intensity: %.2f", lux);
        xSemaphoreGive(xCountingSemaphore);

        vTaskDelay(PUBLISH_INTERVAL);
    }
}

void mqttTask(void *parameter) {
    unsigned long lastPublish = 0;

    ESPMqtt.setServer(mqttServer.c_str(), mqttPort.toInt());
    vTaskDelay(5000);

    for (;;) {
        if (WiFi.status() != WL_CONNECTED) {
            connectToNetwork();
        }

        if (WiFi.status() == WL_CONNECTED && !ESPMqtt.connected()) {
            connectToMqtt();
        }

        if (millis() - lastPublish > PUBLISH_INTERVAL) {
            xSemaphoreTake(xCountingSemaphore, portMAX_DELAY);
            ESP_LOGI("MQTT", "Get sensor data -> Temperature = %.2f C - Humidity = %.2f - Light = %.2f", tempFloat, humidFloat, lux);
            // publishMessage();
            tempFloat = 0;
            humidFloat = 0;
            lux = 0;
            xSemaphoreGive(xCountingSemaphore);

            lastPublish = millis();
        }

        vTaskDelay(PUBLISH_INTERVAL);

        ESPMqtt.loop();
    }
}

boolean updateDhtData() {

    // TO DO: Data Verification
    // Get temperature and humidity data
    tempFloat = dht.readTemperature();
    humidFloat = dht.readHumidity();

    if (tempFloat > 0 && humidFloat > 0) {
        return true;
    } else {
        return false;
    }
}

boolean updateLightData() {

    // TO DO: Data Verification
    // Get temperature and humidity data
    lux = lightMeter.readLightLevel();
    if (lux < 0) {
        return false;
    } else {
        return true;
    }
}

void connectToNetwork() {
    vTaskDelay(10);
    // We start by connecting to a WiFi network
    ESP_LOGI("WIFI", "Connecting to %s", ssid);

    WiFi.begin(ssid, pass);

    while (WiFi.status() != WL_CONNECTED) {
        vTaskDelay(500);
        ESP_LOGI("WIFI", ".");
    }
}

void connectToMqtt() {
    while (!ESPMqtt.connected()) {
        ESP_LOGI("MQTT", "ESP > Connecting to MQTT...");

        if (ESPMqtt.connect("ESP32Client-Nazmi", mqttUser.c_str(), mqttPwd.c_str())) {
            ESP_LOGI("MQTT", "Connected to Server");
        } else {
            ESP_LOGI("MQTT", "ERROR > failed with state %d", ESPMqtt.state());
            delay(2000);
        }
    }
}

void publishMessage() {
    char msgToSend[1024] = {0};
    const size_t capacity = JSON_OBJECT_SIZE(4);
    DynamicJsonDocument doc(capacity);

    String temperature = String(tempFloat);
    String humidity = String(humidFloat);

    doc["eventName"] = "sensorStatus";
    doc["status"] = "none";
    doc["temp"] = temperature.c_str();
    doc["humid"] = humidity.c_str();

    serializeJson(doc, msgToSend);

    ESP_LOGI("MQTT", "Message to publish: %s", msgToSend);
    ESPMqtt.publish(pubTopic.c_str(), msgToSend);
}