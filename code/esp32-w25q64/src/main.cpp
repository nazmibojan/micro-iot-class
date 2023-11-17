#include <Arduino.h>
#include <SPI.h>

#define CS_PIN 5
#define SCK_PIN 18
#define MISO_PIN 19
#define MOSI_PIN 23

#define CHIP_ID 0x9F

#define LEN_ID 4

byte chipId[4] = "";

void chipInit();
void chipGetId();

void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
    chipInit();
    chipGetId();
}

void loop() {
    // put your main code here, to run repeatedly:
}

void chipInit() {
    pinMode(CS_PIN, OUTPUT);
    SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);
    digitalWrite(CS_PIN, HIGH);
}

void chipGetId() {
    // Get chip ID
    digitalWrite(CS_PIN, LOW);
    SPI.transfer(CHIP_ID);
    for (int i = 0; i < LEN_ID; i++) {
        chipId[i] = SPI.transfer(0);
    }
    digitalWrite(CS_PIN, HIGH);
    ESP_LOGI("W25Q64", "Chip ID in bytes: ");
    for (int i = 0; i < LEN_ID; i++) {
        ESP_LOGI("W25Q64", "%X ", chipId[i]);
    }
    Serial.println();
}
