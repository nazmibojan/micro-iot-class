#include <Arduino.h>

int temp[3];
int tempLen, tempAverage;
int *pointerTemp1, *pointerTemp2, *pointerTemp3;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(1000);
  tempLen = sizeof(temp) / sizeof(temp[0]);
  pointerTemp1 = &temp[0];
  pointerTemp2 = &temp[1];
  pointerTemp3 = &temp[2];

  Serial.print("Alamat data pertama: ");
  Serial.println((unsigned int)pointerTemp1);
  Serial.print("Alamat data kedua: ");
  Serial.println((unsigned int)pointerTemp2);
  Serial.print("Alamat data ketiga: ");
  Serial.println((unsigned int)pointerTemp3);
}

void loop() {
  // put your main code here, to run repeatedly:
  for (int x = 0; x < tempLen; x++) {
    temp[x] = random(20,30);
  }

  tempAverage = (*pointerTemp1 + *pointerTemp2 + *pointerTemp3) / tempLen;
  Serial.println(tempAverage);
  delay(5000);
}