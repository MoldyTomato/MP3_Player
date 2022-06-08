#ifndef PushButton_h
#define PushButton_h

#include "Arduino.h"

class PushButton
{
  public:
    //uses pullup
    PushButton(uint8_t pin) {
      _pin = pin;
      pinMode(_pin, INPUT_PULLUP);
      lastSteadyState = 1;
      lastFlickeringState = 1;
    }
    int ButtonStateChange() {
      currentState = digitalRead(_pin);
      if (currentState != lastFlickeringState) {
        lastDebounceTime = millis();
        lastFlickeringState = currentState;
      }
      if ((millis() - lastDebounceTime) > 50) {
        if (lastSteadyState && !currentState) {
          lastSteadyState = currentState;
          return 1; //pressed
        }
        else if (!lastSteadyState && currentState) {
          lastSteadyState = currentState;
          return - 1; //released
        }
        lastSteadyState = currentState;
      }
      if (currentState == LOW) {
        return 2;
      } else
        return - 2;
    }; //returns 1 if pressed -1 if released otherwise +/-2
  private:
    uint8_t _pin;
    bool lastSteadyState;
    bool lastFlickeringState;
    bool currentState;
    //uint8_t DebounceTime = 50;
    unsigned long lastDebounceTime = 0;
};
#endif
