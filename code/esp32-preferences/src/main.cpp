#include <Arduino.h>
#include <Preferences.h>

#define STRING_DEMO_ENABLE

#define STORAGE_NAME "storage"
#define LED_STATUS_KEY "led"
#define SSID_KEY "ssid"
#define BUTTON_PIN 17

bool ledStatus = LOW;
bool changeLedStatus = false;
portMUX_TYPE gpioIntMux = portMUX_INITIALIZER_UNLOCKED;

Preferences storage;
String ssid = "";

void IRAM_ATTR gpioISR() {
    portENTER_CRITICAL(&gpioIntMux);
    changeLedStatus = true;
    portEXIT_CRITICAL(&gpioIntMux);
}

void setup() {
    // put your setup code here, to run once:
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(BUILTIN_LED, OUTPUT);
    attachInterrupt(BUTTON_PIN, &gpioISR, FALLING);

    Serial.begin(115200);

    // Initialize Preferences and read LED status
    storage.begin(STORAGE_NAME);
    delay(100);
    ledStatus = storage.getBool(LED_STATUS_KEY);
    storage.end();
    digitalWrite(BUILTIN_LED, ledStatus);

#ifdef STRING_DEMO_ENABLE
    // Print current SSID
    storage.begin(STORAGE_NAME);
    ssid = storage.getString(SSID_KEY);
    ESP_LOGI("SETUP", "SSID name: %s ", ssid.c_str());
    ssid.clear();
    storage.end();
#endif
}

void loop() {
    // put your main code here, to run repeatedly:
    if (changeLedStatus) {
        portENTER_CRITICAL(&gpioIntMux);
        changeLedStatus = false;
        portEXIT_CRITICAL(&gpioIntMux);

        ledStatus = !ledStatus;
        digitalWrite(BUILTIN_LED, ledStatus);

        storage.begin(STORAGE_NAME);
        storage.putBool(LED_STATUS_KEY, ledStatus);
        storage.end();
    }

#ifdef STRING_DEMO_ENABLE
    if (Serial.available()) {
        ssid = Serial.readStringUntil(';');

        storage.begin(STORAGE_NAME);
        delay(100);
        storage.putString(SSID_KEY, ssid);
        storage.end();

        ESP_LOGI("LOOP", "SSID Name: %s ", ssid.c_str());
    }
#endif
}