#include <Arduino.h>

#define LED1 BUILTIN_LED
#define LED2 4
#define LED3 19
#define LED4 21

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    char data = Serial.read();
    Serial.println();
    Serial.print("Karakter yang dikirim: ");
    Serial.println(data);

    if (data == '1') {
      Serial.println("LED1 On");
      digitalWrite(LED1, HIGH);
      delay(2000);
      digitalWrite(LED1, LOW);
    } else if (data == '2') {
      Serial.println("LED2 On");
      digitalWrite(LED2, HIGH);
      delay(2000);
      digitalWrite(LED2, LOW);
    } else if (data == '3') {
      Serial.println("LED3 On");
      digitalWrite(LED3, HIGH);
      delay(2000);
      digitalWrite(LED3, LOW);
    } else if (data == '4') {
      Serial.println("LED4 On");
      digitalWrite(LED4, HIGH);
      delay(2000);
      digitalWrite(LED4, LOW);
    } else {
      Serial.println("Perintah tidak dikenali");
    }
  }
}
