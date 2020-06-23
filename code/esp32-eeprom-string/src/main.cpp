#include <Arduino.h>
#include <EEPROM.h>

#define EEPROM_SIZE 32

char readData[32], receivedData[32];
int dataIndex = 0;

void readEEPROM(int address, char * data);
void writeEEPROM(int address, char * data);

void setup() {
  Serial.begin(9600);

  // Initialize EEPROM
  EEPROM.begin(EEPROM_SIZE);
  delay(100);
  readEEPROM(0, readData);
  Serial.print("Isi EEPROM: ");
  Serial.println(readData);
}

void loop() {
  if (Serial.available()) {
    receivedData[dataIndex] = Serial.read();
    dataIndex++;

    if (receivedData[dataIndex - 1] == '\n') {
      dataIndex = 0;
      writeEEPROM(0, receivedData);
      memset(receivedData, 0, EEPROM_SIZE);
    }
  }
}

void readEEPROM(int address, char * data) {
  for (int i = 0; i < EEPROM_SIZE; i++) {
    data[i] = EEPROM.read(address + i);
  }
}

void writeEEPROM(int address, char * data) {
  Serial.println("Start Writing to EEPROM");
  for (int i = 0; i < EEPROM_SIZE; i++) {
    EEPROM.write(address + i, data[i]);
  }
  EEPROM.commit();
}