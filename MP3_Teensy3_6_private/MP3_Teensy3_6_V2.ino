#include "PushButton.h"
#include "SwitchON_OFF.h"
#include "RotaryEncoder.h"
#include "StateMachine.h"

//****************************
//ENCODER RIGHT
RotaryEncoder encoder_right(23, 22, RotaryEncoder::LatchMode::FOUR3);//TWO03);
PushButton button_encoder_right(21);
//ENCODER LEFT
RotaryEncoder encoder_left(35, 34, RotaryEncoder::LatchMode::FOUR3);//TWO03);
PushButton button_encoder_left(33);
//SWITCH
PushButton displaySwitch(0);

//*****************************
//StateMachine
StateMachine *SM = new StateMachine();

void setup() {
  Serial.begin(9600);
  //while (!Serial) {}
  // put your setup code here, to run once:
  int x = 0;
  x = SM->initialise();
  if (x == -1) {
    Serial.println("VS1053 Failed");
  } else if (x == -2) {
    Serial.println("SD Card failed");
  } else if(x==-3){
    Serial.println("OLED failed");  
  }  else if (x < 0) {
    Serial.println("Unknown Error message");
  }
  if (x < 0) {
    //SM->RetryMsg();
    for (;;) {}
  }
  Serial.println(x);
}

void loop() {
  int switchState = displaySwitch.ButtonStateChange();
  bool _ON = (switchState == 2) ? true : false;
  if (switchState == -1) {
    //turn off display
    SM->_display->clearDisplay();
    SM->_display->display();
  } else if (switchState == 1) {
    //turn on Display()
    SM->Refresh_Display();
  }
  //encoder right
  encoder_right.tick();
  int d_enc_r = (int)encoder_right.getDirection();
  if (d_enc_r == 1 &&_ON) {
    SM->ScrollDown_EncRight();
    Serial.println("Encoder right increase");
  } else if (d_enc_r == -1 &&_ON) {
    SM->ScrollUp_EncRight();
    Serial.println("Encoder right decrease");
  }
  int b_enc_r = button_encoder_right.ButtonStateChange();
  if (b_enc_r == 1 &&_ON) {
    SM->ButtonDown_EncRight();
    Serial.println("Button Encoder Right Down");
    Serial.print("Number of items:"); Serial.println(SM->_num_items);
    for (int i = 0; i < SM->_num_items; i++) {
      Serial.print(i); Serial.print(":"); Serial.println(SM->_items[i]);
    }
  }
  //encoder left
  encoder_left.tick();
  int d_enc_l = (int)encoder_left.getDirection();
  if (d_enc_l == 1 &&_ON) {
    SM->ScrollDown_EncLeft();
    Serial.println("Encoder left increase");
  } else if (d_enc_l == -1 &&_ON) {
    SM->ScrollUp_EncLeft();
    Serial.println("Encoder left decrease");
  }
  int b_enc_l = button_encoder_left.ButtonStateChange();
  if (b_enc_l == 1 &&_ON) {
    SM->ButtonDown_EncLeft();
    Serial.println("Button Encoder Left Down");
    Serial.print("Number of items:"); Serial.println(SM->_num_items);
    for (int i = 0; i < SM->_num_items; i++) {
      Serial.print(i); Serial.print(":"); Serial.println(SM->_items[i]);
    }
  }
  SM->checkIfPlaying();
}
