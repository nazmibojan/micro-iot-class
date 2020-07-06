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

  Serial2.println("AT"); //Once the handshake test is successful, it will back to OK
  updateSerial();

  Serial2.println("ATD+ +6281394340148;");
  updateSerial();
  delay(20000); // wait for 20 seconds...
  Serial2.println("ATH"); //hang up
  updateSerial();
}

void loop() {
  // put your main code here, to run repeatedly:
}

void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    Serial2.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(Serial2.available()) 
  {
    Serial.write(Serial2.read());//Forward what Software Serial received to Serial Port
  }
}