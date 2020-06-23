#include <Arduino.h>
#include <EEPROM.h>

#define EEPROM_SIZE 32

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
  delay(10000);
}

