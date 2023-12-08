#include <Arduino.h>
#include <DHT.h>
#include <WiFi.h>

#define DHT_PIN 4
#define DHT_TYPE DHT11

DHT dht(DHT_PIN, DHT_TYPE);

const char *ssid = "Praktisi AP";
const char *pass = "semangka";
int lastRequest = 0;

TaskHandle_t dht11TaskHandle;
TaskHandle_t ledTaskHandle;

void setupWifi();

void dht11Task(void *parameter);
void ledTask(void *parameter);

void setup() {
    // put your setup code here, to run once:
    pinMode(BUILTIN_LED, OUTPUT);
    Serial.begin(9600);
    vTaskDelay(1000);

    setupWifi();

    ESP_LOGI("SETUP", "Create freertos task!");

    // Create RTOS task
    xTaskCreate(dht11Task, "DHT11 Task", 2048, NULL, 1, &dht11TaskHandle);
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

void dht11Task(void *parameter) {
    // EventBits_t clientBits;
    float tempFloat = 0.0, humidFloat = 0.0;

    dht.begin();

    for (;;) {
        tempFloat = dht.readTemperature();
        humidFloat = dht.readHumidity();

        ESP_LOGI("SENSOR", "Get sensor data -> Temperature = %.2f C & Humidity = %.2f %", tempFloat, humidFloat);
        vTaskDelay(10000);
    }
}

void ledTask(void *parameter) {
    for (;;) {
        if (WiFi.status() == WL_CONNECTED) {
            digitalWrite(BUILTIN_LED, HIGH);
            vTaskDelay(1000);
            digitalWrite(BUILTIN_LED, LOW);
            vTaskDelay(1000);
        } else {
            digitalWrite(BUILTIN_LED, HIGH);
            vTaskDelay(200);
            digitalWrite(BUILTIN_LED, LOW);
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
