#ifndef SwitchON_OFF_h
#define SwitchON_OFF_h

#include "Arduino.h"

class SwitchON_OFF
{
  public:
    SwitchON_OFF(int pin){
      _pin = pin;
      pinMode(_pin, INPUT);
      lastSteadyState = 1;
      lastFlickeringState=1;
    };
    int getSwitchState(){
      currentState = digitalRead(_pin);
      if(currentState != lastFlickeringState){
        lastDebounceTime = millis();
        lastFlickeringState = currentState;
      }
      if((millis() - lastDebounceTime) >50){
        if(lastSteadyState && !currentState){
          lastSteadyState = currentState;
          return 1; 
        }
        else if(!lastSteadyState && currentState){
          lastSteadyState = currentState;
          return -1;
        }
        lastSteadyState = currentState;
      }
      if(currentState == LOW){
        return 2;
      }else{
        return -2;
      }
    };
  private:
    int _pin;
    bool lastSteadyState;
    bool lastFlickeringState;
    bool currentState;
    unsigned long lastDebounceTime=0;
};
#endif
