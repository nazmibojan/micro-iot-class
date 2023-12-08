#include <Arduino.h>
#include <SPI.h>

#include <w25q64.hpp>

// #define BUTTON_PIN 15

unsigned char writeBuffer[256];
unsigned char readBuffer[256];
byte chipId[4] = "";
bool autoState;

w25q64 spiChip;

void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
    // pinMode(BUTTON_PIN, INPUT_PULLUP);
    spiChip.begin();

    spiChip.getId(chipId);
    // Print ID
    ESP_LOGI("SETUP", "Chip ID in bytes: ");
    for (int i = 0; i < LEN_ID; i++) {
        ESP_LOGI("W25Q64", "%X ", chipId[i]);
    }

    // Menulis data ke chip W25Q64
    // memcpy(writeBuffer, "Praktisi Mengajar UPI", sizeof("Praktisi Mengajar UPI"));
    // spiChip.erasePageSector(0xFFFF);
    // spiChip.pageWrite(writeBuffer, 0xFFFF);
    // ESP_LOGI("SETUP", "Done writing");
    // delay(1000);

    // Baca hasil penulisan data di W25Q64
    ESP_LOGI("SETUP", "Start Reading...");
    spiChip.readPages(readBuffer, 0xFFFF, 1);
    ESP_LOGI("SETUP", "Data read from chip: %s", readBuffer);
}

void loop() {
}
