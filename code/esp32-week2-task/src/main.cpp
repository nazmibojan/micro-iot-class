#include <Arduino.h>

#define POT_PIN 4

#define LED0 18
#define LED1 19
#define LED2 21
#define LED3 22

int adcValue = 0;
int ledList[4] = {LED0, LED1, LED2, LED3};

void ledOn(int ledNumber);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  for (int i = 0; i < 4; i++) {
    pinMode(ledList[i], OUTPUT);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  adcValue = analogRead(POT_PIN);
  Serial.println(adcValue);
  delay(500);

  ledOn(0);
  delay(1000);
  ledOn(1);
  delay(1000);
  ledOn(2);
  delay(1000);
  ledOn(3);
  delay(1000);
}

void ledOn(int ledNumber) {
  for (int i = 0; i < 4; i++) {
    if (i == ledNumber) {
      digitalWrite(ledList[i], HIGH);
    } else {
      digitalWrite(ledList[i], LOW);
    }
  }
}