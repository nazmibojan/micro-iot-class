#include <Arduino.h>
#include <time.h>

// SIM800L Modem Pins
#define MODEM_RST 5
#define MODEM_TX 17
#define MODEM_RX 16

// Set serial for AT commands (to SIM800 module)
#define SerialAT Serial2

// Configure TinyGSM library
#define TINY_GSM_MODEM_SIM800 // Modem is SIM800

// Your GPRS credentials (leave empty, if not needed)
const char apn[] = "AXIS";        // APN
const char gprsUser[] = "axis";   // GPRS User
const char gprsPass[] = "123456"; // GPRS Password

#include <TinyGsmClient.h>

TinyGsm modem(SerialAT);
TinyGsmClient GsmClient(modem);

String timeString;

void gprsSetup();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(MODEM_RST, OUTPUT);
  digitalWrite(MODEM_RST, HIGH);
  SerialAT.begin(38400, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(6000);

  if (!modem.restart()) {
    Serial.println("Modem failed to restart");
  } else {
    Serial.println("Modem successfully restart");
  }
  delay(1000);
  gprsSetup();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (modem.isGprsConnected()) {
    timeString = modem.getGSMDateTime(DATE_FULL);
    Serial.println(timeString);
    delay(30000);
  } else {
    Serial.println("Modem is offline. Reconnect modem...");
    gprsSetup();
  }
}

void gprsSetup() {
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    Serial.println("Failed to connect to GPRS network");
  } else {
    Serial.println("Successfully connected to GPRS network");
  }

  if (modem.isGprsConnected()) {
    Serial.println("Modem is online");
  } else {
    Serial.println("Modem is offline");
  }
}
