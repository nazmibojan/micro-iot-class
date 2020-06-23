#include <Arduino.h>

#define LED_CHANNEL 0

// setting PWM properties
const int freq = 5000;
const int resolution = 8;
 
void setup(){
  // configure LED PWM functionalitites
  ledcSetup(LED_CHANNEL, freq, resolution);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(19, LED_CHANNEL);
}
 
void loop(){
  // increase the LED brightness
  for(int dutyCycle = 0; dutyCycle <= 255; dutyCycle++){   
    // changing the LED brightness with PWM
    ledcWrite(LED_CHANNEL, dutyCycle);
    delay(15);
  }

  // decrease the LED brightness
  for(int dutyCycle = 255; dutyCycle >= 0; dutyCycle--){
    // changing the LED brightness with PWM
    ledcWrite(LED_CHANNEL, dutyCycle);   
    delay(15);
  }
}