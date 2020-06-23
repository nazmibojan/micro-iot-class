#include <Arduino.h>
#include <SPI.h>
#include <w25q64.hpp>

unsigned char writePage[256];
unsigned char readPage[256];
byte chipId[4] = "";

w25q64 spiChip;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  spiChip.begin();
  
  spiChip.getId(chipId);
  // Print ID
  Serial.print("Chip ID in bytes: ");
  for (int i = 0; i < LEN_ID; i++) {
    Serial.print(chipId[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  // Menulis data ke chip W25Q64
  memcpy(writePage, "Nusantech Academy", sizeof("Nusantech Academy"));
  spiChip.erasePageSector(0xFFFF);
  spiChip.pageWrite(writePage, 0xFFFF);
  Serial.println("Done writing");
  delay(1000);

  // Baca hasil penulisan data di W25Q64
  Serial.println("Start Reading...");
  spiChip.readPages(readPage, 0xFFFF, 1);
  Serial.print("Data read from chip: ");
  for (int i = 0; i < 256; i++) {
    if (readPage[i] > 7 && readPage[i] < 127)
      Serial.print((char)readPage[i]);
  }
  Serial.println();
}

void loop() {
  // put your main code here, to run repeatedly:
}
