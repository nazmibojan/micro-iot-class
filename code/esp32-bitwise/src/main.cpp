#include <Arduino.h>

byte sensorData[2] = "";
byte counter = 0;
unsigned short tempShort;
float tempFloat;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    sensorData[counter] = Serial.read();
    counter++;
    if (counter > 1) {
      counter = 0;
      tempShort = (unsigned short)sensorData[0] & 0xFF;
      tempShort = tempShort | ((unsigned short)(sensorData[1] << 8) & 0xFF);
      tempFloat = (float)tempShort / 10;
      
      String data = "Nilai suhu sensor: " + String(tempFloat);
      Serial.println(data);
    }
  }
}

