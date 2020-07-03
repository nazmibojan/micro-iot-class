#include <Arduino.h>
#include <time.h>

// SIM800L Modem Pins
#define MODEM_RST 5
#define MODEM_TX 17
#define MODEM_RX 16

#define GSM_DEST "+6283822767252"

// Set serial for AT commands (to SIM800 module)
#define SerialAT Serial2

// Configure TinyGSM library
#define TINY_GSM_MODEM_SIM800 // Modem is SIM800

#define GSM_AUTOBAUD_MIN 9600
#define GSM_AUTOBAUD_MAX 115200

// Your GPRS credentials (leave empty, if not needed)
// const char apn[] = "AXIS";        // APN
// const char gprsUser[] = "axis";   // GPRS User
// const char gprsPass[] = "123456"; // GPRS Password

#include <TinyGsmClient.h>

TinyGsm modem(SerialAT);
TinyGsmClient GsmClient(modem);

void updateSerial();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(MODEM_RST, OUTPUT);
  digitalWrite(MODEM_RST, HIGH);
  SerialAT.begin(38400, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(6000);

  // TinyGsmAutoBaud(SerialAT, GSM_AUTOBAUD_MIN, GSM_AUTOBAUD_MAX);
  // delay(1000);

  if (!modem.restart()) {
    Serial.println("Modem failed to restart");
  } else {
    Serial.println("Modem successfully restart");
  }
  delay(1000);

  // Send sms
  String text = "Hello World!";
  // modem.sendSMS("+6283822767252", &text[0]);
  if(modem.sendSMS(GSM_DEST, "Hello World!")) {
    Serial.println("Successfully sent SMS...");
  } else {
    Serial.println("Failed to send SMS");
  }
  // modem.sendSMS_UTF16("+380679837464", u"Hello World!", 12);
  
  // Send SMS another way
  delay(1000);

  Serial2.println("AT"); //Once the handshake test is successful, it will back to OK
  updateSerial();

  Serial2.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  Serial2.println("AT+CMGS=\"+6283822767252\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
  updateSerial();
  Serial2.print("Hello World!"); //text content
  updateSerial();
  Serial2.write(26);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1);
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