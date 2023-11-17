#include <Arduino.h>
#include <EEPROM.h>

#define EEPROM_SIZE 1
#define BUTTON_PIN 21

unsigned char led_status = LOW;
bool changeLedStatus = false;
portMUX_TYPE gpioIntMux = portMUX_INITIALIZER_UNLOCKED;

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

    // Initialize EEPROM and read LED status
    EEPROM.begin(EEPROM_SIZE);
    delay(100);
    led_status = EEPROM.read(0);
    digitalWrite(BUILTIN_LED, led_status);
}

void loop() {
    // put your main code here, to run repeatedly:
    if (changeLedStatus) {
        portENTER_CRITICAL(&gpioIntMux);
        changeLedStatus = false;
        portEXIT_CRITICAL(&gpioIntMux);

        led_status = !led_status;
        digitalWrite(BUILTIN_LED, led_status);

        EEPROM.write(0, led_status);
        EEPROM.commit();
    }
}
