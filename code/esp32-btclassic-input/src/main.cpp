#include <Arduino.h>
#include <BluetoothSerial.h>

BluetoothSerial SerialBT;

String receivedString;

void setup() {
  // put your setup code here, to run once:
  SerialBT.begin("ESP32-Test");
  pinMode(BUILTIN_LED, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (SerialBT.available()) {
    char receivedChar = SerialBT.read();
    receivedString += receivedChar;

    if (receivedChar == '@') {
      if (receivedString == "led on@") {
        SerialBT.println("Turn On LED");
        digitalWrite(BUILTIN_LED, HIGH);
      } else if (receivedString == "led off@") {
        SerialBT.println("Turn Off LED");
        digitalWrite(BUILTIN_LED, LOW);
      } else {
        SerialBT.println("Could not recognized command");
      }

      receivedString = "";
      SerialBT.flush();
    }
  }
}