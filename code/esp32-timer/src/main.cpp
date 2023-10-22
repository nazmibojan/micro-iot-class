#include <Arduino.h>

volatile bool interruptState = false;
int totalInterruptCounter;

hw_timer_t* timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer() {
    portENTER_CRITICAL(&timerMux);
    interruptState = true;
    portEXIT_CRITICAL(&timerMux);
}

void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);

    // Timer initialization
    timer = timerBegin(0, 8000, true);  // clock timer = 80000000 / 8000 = 10000 Hz
    timerAttachInterrupt(timer, &onTimer, true);
    timerAlarmWrite(timer, 10000, true);  // Output akhir 10000 / 10000 = 1 Hz -> Periode = 1 s
    timerAlarmEnable(timer);
}

void loop() {
    // put your main code here, to run repeatedly:
    if (interruptState) {
        portENTER_CRITICAL(&timerMux);
        interruptState = false;
        portEXIT_CRITICAL(&timerMux);

        totalInterruptCounter++;
        ESP_LOGI("TIMER", "Total interrupt happened: %d", totalInterruptCounter);
        // Serial.println(sentence);
    }
}