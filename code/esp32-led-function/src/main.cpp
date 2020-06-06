#include <Arduino.h>

void initLed();
void ledOn();
void ledOff();

void setup() {
  // put your setup code here, to run once:
  initLed();
}

void loop() {
  // put your main code here, to run repeatedly:
  ledOn();
  delay(1000);
  ledOff();
  delay(1000);
}

void initLed() {
  pinMode(BUILTIN_LED, OUTPUT);
}

void ledOn() {
  digitalWrite(BUILTIN_LED, HIGH);
}

void ledOff() {
  digitalWrite(BUILTIN_LED, LOW);
}

