#include <Arduino.h>

void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
    delay(1000);
}

void loop() {
    // put your main code here, to run repeatedly:
    if (Serial.available()) {
        char data = Serial.read();
        ESP_LOGI("UART RECEIVE", "Karakter yang dikirim: %c", data);
        // Serial.println(data);
    }
}