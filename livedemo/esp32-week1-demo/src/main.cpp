#include <Arduino.h>

#define MODEM_RST 15
#define MODEM_TX 17
#define MODEM_RX 16
#define SerialAT Serial2

void setup() {
  Serial.begin(9600);
  pinMode(MODEM_RST, OUTPUT);
  digitalWrite(MODEM_RST, HIGH); // harus high dulu utk pertama
  SerialAT.begin(38400, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(6000);
}

void loop() {
  if (SerialAT.available())
    Serial.write(SerialAT.read());

  if (Serial.available())
    SerialAT.write(Serial.read());
}