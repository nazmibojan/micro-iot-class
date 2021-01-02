#include <Arduino.h>
#include <EEPROM.h>
#inclusw <HTTPUpdate.h>

#define EEPROM_SIZE 32

#define SSID_ADDRESS 0
#define SSID_SIZE 10
#define PASS_ADDRESS 10
#define PASS_SIZE 10

char ssid[SSID_SIZE];
char password[PASS_SIZE];

char writeChar[32] = "Nusantech Academy";
int len = 0;

void setup() {
  Serial.begin(9600);
  len = sizeof(writeChar);

  // Initialize EEPROM
  Serial.println("Start writing EEPROM");
  EEPROM.begin(EEPROM_SIZE);
  delay(100);
  // Write char to EEPROM
  for (int i = 0; i < len; i++) {
    EEPROM.write(i, writeChar[i]);
  }
  EEPROM.commit();
  Serial.println("Writing is done!");
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("Data EEPROM from address 0: ");
  for (int i = 0; i < len; i++) {
    char readChar = EEPROM.read(i);
    Serial.print(readChar);
  }
  Serial.println();

  // Write SSID
  memset(ssid, 0, SSID_SIZE);
  memcpy(ssid, "wifi", 4);


  delay(10000);
}

