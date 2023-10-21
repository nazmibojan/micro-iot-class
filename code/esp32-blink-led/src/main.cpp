#include <Arduino.h>

// put function declarations here:
// int myFunction(int, int);
#define LED_PIN 2
#define LED_PIN_EX 4

void setup() {
    // put your setup code here, to run once:
    // int result = myFunction(2, 3);
    pinMode(LED_PIN, OUTPUT);
    pinMode(LED_PIN_EX, OUTPUT);
}

void loop() {
    // put your main code here, to run repeatedly:
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(LED_PIN_EX, LOW);
    delay(2000);
    digitalWrite(LED_PIN, LOW);
    digitalWrite(LED_PIN_EX, HIGH);
    delay(2000);
}

// put function definitions here:
// int myFunction(int x, int y) {
//   return x + y;
// }