#include <Arduino.h>

void setup() {
  // put your setup code here, to run once:
  pinMode(2, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(2, HIGH);
  delay(2000);
  digitalWrite(2, LOW);
  delay(2000);
}

