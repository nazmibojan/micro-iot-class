#include <Arduino.h>

#define LED1 BUILTIN_LED
#define LED2 4

void initLed(int ledNumber) {
  pinMode(ledNumber, OUTPUT);
}

void ledOn(int ledNumber) {
  digitalWrite(ledNumber, HIGH);
}

void ledOff(int ledNumber) {
  digitalWrite(ledNumber, LOW);
}

void setup() {
  // put your setup code here, to run once:
  initLed(LED1);
  initLed(LED2);
}

void loop() {
  // put your main code here, to run repeatedly:
  ledOn(LED1);
  ledOff(LED2);
  delay(1000);
  ledOff(LED1);
  ledOn(LED2);
  delay(1000);
}