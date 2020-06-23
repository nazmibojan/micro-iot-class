#include <Arduino.h>

#define LED_PIN 18
#define LED_CHANNEL 0
#define PWM_FREQ 5000
#define PWM_RESOLUTION 8
 
void setup(){
  // configure LED PWM functionalitites
  ledcSetup(LED_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(LED_PIN, LED_CHANNEL);
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