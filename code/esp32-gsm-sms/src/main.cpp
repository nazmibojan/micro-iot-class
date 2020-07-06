#include <Arduino.h>

// SIM800L Modem Pins
#define MODEM_RST 5
#define MODEM_TX 17
#define MODEM_RX 16

// Set serial for AT commands (to SIM800 module)
#define SerialAT Serial2

void updateSerial();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(MODEM_RST, OUTPUT);
  digitalWrite(MODEM_RST, HIGH);
  SerialAT.begin(38400, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(6000);

  Serial2.println("AT");
  updateSerial();

  Serial2.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  Serial2.println("AT+CMGS=\"+6288295861359\"");
  updateSerial();
  Serial2.print("Hello World!"); // text content
  updateSerial();
  Serial2.write(26);
}

void loop() {}

void updateSerial() {
  delay(500);
  while (Serial2.available()) {
    Serial.write(Serial2.read());
  }
}