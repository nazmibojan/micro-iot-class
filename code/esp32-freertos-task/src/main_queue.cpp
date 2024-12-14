#include <Arduino.h>
// #include <DHT.h>

#include <main.hpp>

#include <Wire.h>
#include <WiFi.h>

#include <Adafruit_BME680.h>
#include <Adafruit_Sensor.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

typedef struct {
  float temperature;
  float humidity;
  float pressure;
} SensorData;

Adafruit_BME680 bme;

const char *ssid = "Foqa Lab";
const char *pass = "Heisenberg1932";
int lastRequest = 0;

// ThingsBoard MQTT Configuration
const char* THINGSBOARD_SERVER = "iot.ferbos-software.com";
const int THINGSBOARD_PORT = 1883;
const char* ACCESS_TOKEN = "0xa4c138182c78dfda";

TaskHandle_t bmeTaskHandle;
TaskHandle_t ledTaskHandle;

QueueHandle_t sensorDataQueue;

WiFiClient espClient;
PubSubClient mqttClient(espClient);

bool bmeBegin();
void setupWifi();
void reconnectMQTT();
void bmeTask(void *parameter);
void ledTask(void *parameter);
void networkConnectionTask(void *pvParameters);
void dataPublishTask(void *pvParameters);

void setup() {
    // put your setup code here, to run once:
    pinMode(LED_PIN, OUTPUT);
    Serial.begin(115200);
    vTaskDelay(1000);

    sensorDataQueue = xQueueCreate(10, sizeof(SensorData));

    mqttClient.setServer(THINGSBOARD_SERVER, THINGSBOARD_PORT);

    ESP_LOGI("SETUP", "Create freertos task!");
    // Create RTOS task
    xTaskCreate(bmeTask, "BME Task", 2048, NULL, 1, &bmeTaskHandle);
    xTaskCreate(ledTask, "LED Task", 2048, NULL, 2, &ledTaskHandle);
    xTaskCreate(
        networkConnectionTask,
        "Network Connection Task",
        4096,
        NULL,
        3,
        NULL
    );

    xTaskCreate(
        dataPublishTask,
        "Data Publish Task",
        4096,
        NULL,
        4,
        NULL
    );
}

void loop() {
    // // put your main code here, to run repeatedly:
    // if (millis() - lastRequest > 10000) {
    //     if (WiFi.status() != WL_CONNECTED) {
    //         setupWifi();
    //     } else {
    //         ESP_LOGI("WIFI", "WiFi is already connected...");
    //     }

    //     lastRequest = millis();
    // }
}

void bmeTask(void *parameter) {
    SensorData data;
    // float tempFloat = 0.0, humidFloat = 0.0;
    // float pressureFloat = 0.0;

    // dht.begin();
    if (bmeBegin()) {
        ESP_LOGI("SETUP", "Successfully init BME sensor");
    }

    for (;;) {
        if (!bme.performReading()) {
            ESP_LOGE("BME", "Failed to perform reading");
        }
        data.temperature = bme.temperature;
        data.humidity = bme.humidity;
        data.pressure = bme.pressure;

        ESP_LOGI("SENSOR", "Get sensor data -> Temperature = %.2f C & Humidity = %.2f % & Pressure = %d hPa", data.temperature, data.humidity, data.pressure);

        if(xQueueSend(sensorDataQueue, &data, portMAX_DELAY) != pdTRUE) {
            ESP_LOGE("SENSOR", "Failed to send data to queue");
        }
        vTaskDelay(10000);
    }
}

void ledTask(void *parameter) {
    for (;;) {
        if (WiFi.status() == WL_CONNECTED) {
            digitalWrite(LED_PIN, HIGH);
            vTaskDelay(1000);
            digitalWrite(LED_PIN, LOW);
            vTaskDelay(1000);
        } else {
            digitalWrite(LED_PIN, HIGH);
            vTaskDelay(200);
            digitalWrite(LED_PIN, LOW);
            vTaskDelay(200);
        }
    }
}

// Network Connection Task
void networkConnectionTask(void *pvParameters) {
  ESP_LOGI("PUBLISH", "Start Publish Data Task.....");

  while(1) {
    // Reconnect WiFi if disconnected
    if(WiFi.status() != WL_CONNECTED) {
      setupWifi();
    }

    // Reconnect MQTT if disconnected
    if(!mqttClient.connected()) {
      reconnectMQTT();
    }

    mqttClient.loop(); // Maintain MQTT connection
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

// Data Publishing Task
void dataPublishTask(void *pvParameters) {
  SensorData receivedData;
  StaticJsonDocument<256> jsonDoc;
  char jsonBuffer[256];

  ESP_LOGI("PUBLISH", "Start Publish Data Task.....");

  while(1) {
    // Try to receive data from queue
    if (mqttClient.connected()) {
        if(xQueueReceive(sensorDataQueue, &receivedData, portMAX_DELAY) == pdTRUE) {
            // Clear previous JSON
            jsonDoc.clear();

            jsonDoc["temperature"] = receivedData.temperature;
            jsonDoc["humidity"] = receivedData.humidity;
            jsonDoc["pressure"] = receivedData.pressure;

            // Serialize JSON to string
            serializeJson(jsonDoc, jsonBuffer);
            ESP_LOGI("PUBLISH", "Publish sensor data: %s", jsonBuffer);

            // Publish to ThingsBoard
            if(mqttClient.publish("v1/devices/me/telemetry", jsonBuffer)) {
                ESP_LOGI("MQTT", "Data Published Successfully");
            } else {
                ESP_LOGE("MQTT", "Publish Failed");
            }
        }
    } else {
        ESP_LOGW("PUBLISH", "Dont publish. MQTT is disconnected");
    }

    // Delay to prevent overwhelming the network
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

void reconnectMQTT() {
  while(!mqttClient.connected()) {
    ESP_LOGI("MQTT", "Attempting MQTT connection...");
    
    if(mqttClient.connect("ESP32Client", ACCESS_TOKEN, "")) {
      ESP_LOGI("MQTT", "MQTT Connected");
    } else {
      ESP_LOGE("MQTT", "Failed. Retrying in 5 seconds");
      vTaskDelay(pdMS_TO_TICKS(5000));
    }
  }
}

void setupWifi() {
    vTaskDelay(10);
    // We start by connecting to a WiFi network
    ESP_LOGI("WIFI", "Connecting to %s", ssid);

    WiFi.begin(ssid, pass);

    while (WiFi.status() != WL_CONNECTED) {
        vTaskDelay(500);
        ESP_LOGI("WIFI", ".");
    }
    ESP_LOGI("WIFI", "WiFi is connected!");
}

bool bmeBegin() {
#if defined(FERBOS_ESP32_V1) || defined(FERBOS_ESP32_V2)
    ESP_LOGI("SENSOR", "Change I2C Pin before initialize BME680");
    Wire.begin(BME680_SDA_PIN, BME680_SCL_PIN);
    vTaskDelay(3000 / portTICK_PERIOD_MS);

    // Initialize the sensor
#ifdef FERBOS_ESP32_V1
    if (!bme.begin(BME680_ALT_ADDRESS, false)) {
#else
    if (!bme.begin()) {
#endif
        ESP_LOGE("SENSOR", "Could not find a valid BME680 sensor, check wiring!");
        return false;
    }
#else
    // Initialize the sensor
    if (!bme.begin()) {
        ESP_LOGE("SENSOR", "Could not find a valid BME680 sensor, check wiring!");
        return false;
    }
#endif

    // Set up oversampling and filter initialization
    bme.setTemperatureOversampling(BME680_OS_8X);
    bme.setHumidityOversampling(BME680_OS_2X);
    bme.setPressureOversampling(BME680_OS_4X);
    bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
    bme.setGasHeater(320, 150);  // 320*C for 150 ms

    ESP_LOGI("SENSOR", "Successfully init BME 680 sensor");

    return true;
}