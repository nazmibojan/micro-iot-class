#include <Arduino.h>
// #include <DHT.h>

#include <main.hpp>

#include <Wire.h>
#include <WiFi.h>

#include <Adafruit_BME680.h>
#include <Adafruit_Sensor.h>

Adafruit_BME680 bme;

const char *ssid = "Foqa Lab";
const char *pass = "Heisenberg1932";
int lastRequest = 0;

TaskHandle_t bmeTaskHandle;
TaskHandle_t ledTaskHandle;

bool bmeBegin();
void setupWifi();
void bmeTask(void *parameter);
void ledTask(void *parameter);

void setup() {
    // put your setup code here, to run once:
    pinMode(LED_PIN, OUTPUT);
    Serial.begin(115200);
    vTaskDelay(1000);

    setupWifi();

    ESP_LOGI("SETUP", "Create freertos task!");
    // Create RTOS task
    xTaskCreate(bmeTask, "BME Task", 2048, NULL, 1, &bmeTaskHandle);
    xTaskCreate(ledTask, "LED Task", 2048, NULL, 2, &ledTaskHandle);
}

void loop() {
    // put your main code here, to run repeatedly:
    if (millis() - lastRequest > 10000) {
        if (WiFi.status() != WL_CONNECTED) {
            setupWifi();
        } else {
            ESP_LOGI("WIFI", "WiFi is already connected...");
        }

        lastRequest = millis();
    }
}

void bmeTask(void *parameter) {
    // EventBits_t clientBits;
    float tempFloat = 0.0, humidFloat = 0.0;

    // dht.begin();
    if (bmeBegin()) {
        ESP_LOGI("SETUP", "Successfully init BME sensor");
    }

    for (;;) {
        if (!bme.performReading()) {
            ESP_LOGE("BME", "Failed to perform reading");
        }
        tempFloat = bme.temperature;
        humidFloat = bme.humidity;

        ESP_LOGI("SENSOR", "Get sensor data -> Temperature = %.2f C & Humidity = %.2f %", tempFloat, humidFloat);
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