//OLED************************************
#include <Adafruit_SSD1306.h>
#include <Wire.h>
//OLED SCL: Pin 19
//OLED SDA: Pin 18
//VS1053 Breakout************************
#include <Adafruit_VS1053.h>
#include <SPI.h>
#include <SD.h>
//#include "SdFat.h"
//SdFat SD;
//VS1053_Breakout CLK(SCLK): Pin 13
//VS1053_Breakout MOSI: Pin 11
//VS1053_Breakout MISO: Pin 12
#define RST 9 //VS1053_Breakout RST: Pin 9
#define CS 10 //VS1053_Breakout CS: Pin 10
#define XDCS 8 //VS1053_Breakout XDCS: Pin 8
#define DREQ 3 //VS1053_Breakout DREQ: Pin 3
#define SDCS 4 //VS1053_Breakout SDCS: Pin 4

class State; //forward declaration
//StateMachine*****************************
class StateMachine {
  public:
    int initialise();
    void RetryMsg();
    void ScrollUp_EncRight();
    void ScrollDown_EncRight();
    void ButtonDown_EncRight();
    void ScrollUp_EncLeft();
    void ScrollDown_EncLeft();
    void ButtonDown_EncLeft();
    void Refresh_Display();
    int _num_items;
    char _items[255][255]={{0}};
    char _root[255] = "/";
    char _interpret[255] = "";
    int _interpret_top_item = 0;
    int _interpret_screen_item = 0;
    int _album_top_item = 0;
    int _album_screen_item = 0;
    char _album[255] = "";
    int _track_top_item = 0;
    int _track_screen_item = 0;
    char _interpret_playing[255]="";
    char _album_playing[255]="";
    char _track_playing[255]="";
    char _tracks[255][255]={{0}};
    int _trackCount;
    int _trackNumber;
    char _options[255][255]={
      "Volume",
      "Repeat",
      "Dim Display",
      "Filename"
    };
    bool _dimDisplay = false;
    bool _truncateFilename=false;
    int _num_options=4;
    int _option_top_item=0;
    int _option_screen_item=0;
    int _volume=255-20;
    int _repeat_mode=1; //0: no repeat, 1: repeat album, 2: repeat track, 3: random
    Adafruit_SSD1306 *_display;
    void ChangeState(State*);
    Adafruit_VS1053_FilePlayer* _musicPlayer;
    void playNextTrack();
    void playPreviousTrack();
    //bool _wasPlaying=false;
    int _play_option_item;
    void checkIfPlaying();
    void playNext();
  private:
    State* _state;

    
};
//State***************************************
class State {
  public:
    virtual void ScrollUp_EncRight(StateMachine*);
    virtual void ScrollDown_EncRight(StateMachine*);
    virtual void ButtonDown_EncRight(StateMachine*);
    virtual void ScrollUp_EncLeft(StateMachine*);
    virtual void ScrollDown_EncLeft(StateMachine*);
    virtual void ButtonDown_EncLeft(StateMachine*);
    virtual void Refresh_Display(StateMachine*);
  private:
  protected:
    void ChangeState(StateMachine*, State*);
};
//State_Interpret********************************
class State_Interprets : public State {
  public:
    static State_Interprets* Instance();
    virtual void ScrollUp_EncRight(StateMachine*);
    virtual void ScrollDown_EncRight(StateMachine*);
    virtual void ButtonDown_EncRight(StateMachine*);
    virtual void ScrollUp_EncLeft(StateMachine*);
    virtual void ScrollDown_EncLeft(StateMachine*);
    virtual void ButtonDown_EncLeft(StateMachine*);
    virtual void Refresh_Display(StateMachine*);
  private:
    static State_Interprets* _instance;
};
//State_Albums************************************
class State_Albums : public State {
  public:
    static State_Albums* Instance();
    virtual void ScrollUp_EncRight(StateMachine*);
    virtual void ScrollDown_EncRight(StateMachine*);
    virtual void ButtonDown_EncRight(StateMachine*);
    virtual void ScrollUp_EncLeft(StateMachine*);
    virtual void ScrollDown_EncLeft(StateMachine*);
    virtual void ButtonDown_EncLeft(StateMachine*);
    virtual void Refresh_Display(StateMachine*);
  private:
    static State_Albums* _instance;
};
//State_Albums_subfolder**************************
//State_Tracks************************************
class State_Tracks : public State {
  public:
    static State_Tracks* Instance();
    virtual void ScrollUp_EncRight(StateMachine*);
    virtual void ScrollDown_EncRight(StateMachine*);
    virtual void ButtonDown_EncRight(StateMachine*);
    virtual void ScrollUp_EncLeft(StateMachine*);
    virtual void ScrollDown_EncLeft(StateMachine*);
    virtual void ButtonDown_EncLeft(StateMachine*);
    virtual void Refresh_Display(StateMachine*);
  public:
    static State_Tracks* _instance;
};
//State_Playing*************************************
class State_Playing : public State {
  public:
    static State_Playing* Instance();
    virtual void ScrollUp_EncRight(StateMachine*);
    virtual void ScrollDown_EncRight(StateMachine*);
    virtual void ButtonDown_EncRight(StateMachine*);
    virtual void ScrollUp_EncLeft(StateMachine*);
    virtual void ScrollDown_EncLeft(StateMachine*);
    virtual void ButtonDown_EncLeft(StateMachine*);
    virtual void Refresh_Display(StateMachine*);
  public:
    static State_Playing* _instance;
};
//State_Options*************************************
class State_Options : public State {
  public:
    static State_Options* Instance();
    virtual void ScrollUp_EncRight(StateMachine*);
    virtual void ScrollDown_EncRight(StateMachine*);
    virtual void ButtonDown_EncRight(StateMachine*);
    virtual void ScrollUp_EncLeft(StateMachine*);
    virtual void ScrollDown_EncLeft(StateMachine*);
    virtual void ButtonDown_EncLeft(StateMachine*);
    virtual void Refresh_Display(StateMachine*);
  public:
    static State_Options* _instance;
};
