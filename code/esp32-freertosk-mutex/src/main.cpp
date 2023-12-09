#include <Arduino.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <PubSubClient.h>
#include <WiFi.h>

#define DHT_PIN 4
#define DHT_TYPE DHT11

#define PUBLISH_INTERVAL 1000

const char *ssid = "NZM IoT Lab";
const char *pass = "Heisenberg1932";
int lastRequest = 0;
float tempFloat, humidFloat;

TaskHandle_t sensorHandle;
TaskHandle_t mqttTaskHandle;

SemaphoreHandle_t xMutexSensor = NULL;

DHT dht(DHT_PIN, DHT_TYPE);

String mqttServer = "broker.hivemq.com";
String mqttUser = "";
String mqttPwd = "";
String deviceId = "Home_Gateway_1";
String pubTopic = String(deviceId + "/sensor_data_nzm");
String mqttPort = "1883";

WiFiClient ESPClient;
PubSubClient ESPMqtt(ESPClient);

void sensorTask(void *parameter);
void mqttTask(void *parameter);

boolean updateDhtData();
void connectToMqtt();
void connectToNetwork();
void publishMessage();

void setup() {
    // put your setup code here, to run once:
    pinMode(BUILTIN_LED, OUTPUT);
    Serial.begin(9600);
    vTaskDelay(1000);

    xMutexSensor = xSemaphoreCreateMutex();

    connectToNetwork();

    // Create RTOS task
    xTaskCreate(sensorTask, "Sensor Task", 2048, NULL, 1, &sensorHandle);
    xTaskCreate(mqttTask, "MQTT Task", 2048, NULL, 2, &mqttTaskHandle);
}

void loop() {
}

void sensorTask(void *parameter) {
    for (;;) {
        xSemaphoreTake(xMutexSensor, portMAX_DELAY);
        while (!updateDhtData()) {
            ESP_LOGI("SENSOR", "Sensor error! Retry in 3 seconds");
            vTaskDelay(3000);
        }
        xSemaphoreGive(xMutexSensor);

        vTaskDelay(PUBLISH_INTERVAL);
    }
}

void mqttTask(void *parameter) {
    unsigned long lastPublish = 0;

    ESPMqtt.setServer(mqttServer.c_str(), mqttPort.toInt());

    for (;;) {
        if (WiFi.status() != WL_CONNECTED) {
            connectToNetwork();
        }

        if (WiFi.status() == WL_CONNECTED && !ESPMqtt.connected()) {
            connectToMqtt();
        }

        if (millis() - lastPublish > PUBLISH_INTERVAL) {
            xSemaphoreTake(xMutexSensor, portMAX_DELAY);
            ESP_LOGI("SENSOR", "Get sensor data -> Temperature = %.2f C & Humidity = %.2f %", tempFloat, humidFloat);
            // publishMessage();
            tempFloat = 0;
            humidFloat = 0;
            xSemaphoreGive(xMutexSensor);

            lastPublish = millis();
        }

        vTaskDelay(200);

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

        if (ESPMqtt.connect("ESP-NazmiFebrian", mqttUser.c_str(), mqttPwd.c_str())) {
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