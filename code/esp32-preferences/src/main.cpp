#include <Arduino.h>
#include <Preferences.h>

#define EEPROM_SIZE 1
#define BUTTON_PIN 21

unsigned char ledStatus = LOW;
bool changeLedStatus = false;
portMUX_TYPE gpioIntMux = portMUX_INITIALIZER_UNLOCKED;

Preferences storage;

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

  // Initialize preferences and read LED status
  storage.begin("Storage", true);
  delay(100);
  ledStatus = storage.getUChar("led_state");
  digitalWrite(BUILTIN_LED, ledStatus);
  storage.end();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (changeLedStatus) {
    portENTER_CRITICAL(&gpioIntMux);
    changeLedStatus = false;
    portEXIT_CRITICAL(&gpioIntMux);

    ledStatus = !ledStatus;
    digitalWrite(BUILTIN_LED, ledStatus);

    storage.begin("Storage", true);
    storage.putUChar("led_state", ledStatus);
    storage.end();
  }
}

