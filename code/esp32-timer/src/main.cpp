#include <Arduino.h>

volatile bool interruptState = false;
int totalInterruptCounter;

hw_timer_t * timer = NULL;
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
  timer = timerBegin(0, 8000, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 10000, true);
  timerAlarmEnable(timer);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (interruptState) {
    portENTER_CRITICAL(&timerMux);
    interruptState = false;
    portEXIT_CRITICAL(&timerMux);

    totalInterruptCounter++;
    String sentence = "Total interrupt happened: " + String(totalInterruptCounter);
    Serial.println(sentence);
  }
}