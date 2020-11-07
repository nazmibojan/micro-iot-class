#include <Arduino.h>
#include <Wire.h>

#define BH175O_ADDRESS 0x23
#define BHI1750_DATALEN 2

void bh1750Request(int address);
int bh1750Read(int address);

byte buff[2];
unsigned short lux = 0;
int x = 0;
float luas1, luas2, luas3;

float hitungVolumeTabung(int radian, int height);

void setup() {
  Wire.begin();
  Serial.begin(9600);
}

void loop() {
  bh1750Request(BH175O_ADDRESS);
  delay(200);

  if (bh1750Read(BH175O_ADDRESS) == BHI1750_DATALEN) {
    lux = (((unsigned short)buff[0] << 8) | (unsigned short)buff[1]) / 1.2;
    Serial.print(lux);
    Serial.println("lux");
  }
  delay(1000);

  function2();

  luas1 = hitungVolumeTabung(4, 5);
  luas2 = hitungVolumeTabung(4, 6);
}

int bh1750Read(int address) //
{
  int i = 0;

  Wire.beginTransmission(address);
  Wire.requestFrom(address, 2);
  while (Wire.available()) //
  {
    buff[i] = Wire.read(); // receive one byte
    i++;
  }
  Wire.endTransmission();

  return i;
}

void bh1750Request(int address) {
  Wire.beginTransmission(address);
  Wire.write(0x10);                 // 1lx reolution 120ms
  Wire.endTransmission();
}


void function1(int x, int y) {

}

void function2() {
  x = 1;
}

String hitungVolumeTabung(int radian, int height) {
  float pi = 3.14;
  float luasFungsi;
  String output;

  luasFungsi = pi * radian * radian * height;

  return luasFungsi;
}