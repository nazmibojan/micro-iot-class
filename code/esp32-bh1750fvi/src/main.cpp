// #define USE_BH1750_LIB

#ifndef USE_BH1750_LIB
#include <Arduino.h>
#include <Wire.h>

#define BH175O_ADDRESS 0x23
#define BHI1750_DATALEN 2

void bh1750Request(int address);
int bh1750Read(int address);

byte buff[2];
unsigned short lux = 0;

void setup() {
    Serial.begin(9600);
    Wire.begin();
    ESP_LOGI("SETUP", "Start Reading Light Sensor..");
}

void loop() {
    bh1750Request(BH175O_ADDRESS);
    delay(200);

    if (bh1750Read(BH175O_ADDRESS) == BHI1750_DATALEN) {
        lux = (((unsigned short)buff[0] << 8) | (unsigned short)buff[1]) / 1.2;
        ESP_LOGI("LIGHT", "Light Intensity: %d", lux);
    } else {
        ESP_LOGE("LIGHT", "Error reading bh1750 sensor");
    }
    delay(1000);
}

int bh1750Read(int address)  //
{
    if (Wire.requestFrom((int)address, (int)2) == 2) {
        buff[0] = Wire.read();
        buff[1] = Wire.read();

        return 2;
    } else {
        return 0;
    }
}

void bh1750Request(int address) {
    Wire.beginTransmission(address);
    Wire.write(0x10);  // 1lx resolution 120ms

    if (Wire.endTransmission() == 0) {
        ESP_LOGI("BH1750", "Success end transmission");
    } else {
        ESP_LOGE("BH1750", "Failed end transmission");
    }
}

#else
/*
  Example of BH1750 library usage. This example initialises the BH1750 object using the default high resolution continuous mode and then makes a light level reading every second.
*/
#include <Arduino.h>
#include <BH1750.h>
#include <Wire.h>

BH1750 lightMeter;

void setup() {
    Serial.begin(9600);

    // Initialize the I2C bus (BH1750 library doesn't do this automatically)
    Wire.begin();
    // On esp8266 you can select SCL and SDA pins using Wire.begin(D4, D3);
    // For Wemos / Lolin D1 Mini Pro and the Ambient Light shield use Wire.begin(D2, D1);

    lightMeter.begin();

    Serial.println(F("BH1750 Test begin"));
}

void loop() {
    float lux = lightMeter.readLightLevel();
    Serial.print("Light: ");
    Serial.print(lux);
    Serial.println(" lx");
    delay(1000);
}

#endif