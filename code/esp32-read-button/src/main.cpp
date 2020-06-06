#include <Arduino.h>

#define BUTTON_PIN 18

int led_status = LOW;

void setup() {
  // put your setup code here, to run once:
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUILTIN_LED, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (digitalRead(BUTTON_PIN) == LOW) {
    led_status = !led_status;
    digitalWrite(BUILTIN_LED, led_status);
    delay(2000);
  }
}
