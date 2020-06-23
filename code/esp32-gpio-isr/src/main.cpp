#include <Arduino.h>

#define BUTTON_PIN 21

int led_status = LOW;

void IRAM_ATTR gpioISR() {
  led_status = !led_status;
  digitalWrite(BUILTIN_LED, led_status);
  delay(2000);
}

void setup() {
  // put your setup code here, to run once:
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUILTIN_LED, OUTPUT);
  attachInterrupt(BUTTON_PIN, &gpioISR, FALLING);
}

void loop() {
  // put your main code here, to run repeatedly:
}