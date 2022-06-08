#include "StateMachine.h"
#include "ItemsToIgnore.h"


int StateMachine::initialise() {
  _display = new Adafruit_SSD1306(128, 64, &Wire, -1, 100000000); //OLED
  _display->begin(SSD1306_SWITCHCAPVCC, 0x3C);
  _display->cp437(true);//true);
  _display->setFont(NULL);
  _display->clearDisplay();
  _display->setTextColor(WHITE);
  _display->setTextSize(1);
  _display->setTextWrap(false);
  //check different components
  //OLED
  _display->setCursor(0, 0);
  _display->println("Initialising:");
  _display->println("OLED...done");
  //VS1053 Breakout
  _display->print("VS1053...");
  _display->display();
  _musicPlayer = new Adafruit_VS1053_FilePlayer(RST, CS, XDCS, DREQ, SDCS);
  if (!_musicPlayer->begin()) {
    _display->println("failed");
    _display->display();
    return -1;
  }
  _display->println("done");
  _display->display();
  //SD Card
  _display->print("SD Card...");
  _display->display();
  //SD CARD
  if (!SD.begin(SDCS)) {
    _display->println("failed"); 
    _display->display();
    return -2;
  }
  _display->println("done");
  _display->display();
  _display->println("SUCCESS");
  
  _display->display();

  _musicPlayer->setVolume(255-_volume, 255-_volume);
  _musicPlayer->useInterrupt(VS1053_FILEPLAYER_PIN_INT);
  _musicPlayer->sineTest(0x44, 500);
  delay(500);
  //count items at root
  int count = 0;
  File dir = SD.open(_root);
  while (true && count < 255) {
    File entry = dir.openNextFile();
    if (!entry) {
      break;
    }
    //check if ignored
    if (isIgnored(entry.name()) == 1) {
      continue;
    }
    //check if folder
    if (entry.isDirectory()) {
      strcpy(_items[count], entry.name());
      count++;
    }
    entry.close();
  }
  dir.close();
  //TODO sort

  _num_items = count;
  _interpret_top_item = 0;
  _interpret_screen_item = 0;
  _state = State_Interprets::Instance();
  this->Refresh_Display();

  return 0;
}
void StateMachine::RetryMsg() {
  _display->fillRect(0, 8 + 8 * 5, 128, 8, SSD1306_WHITE);
  _display->setCursor(0, 8 + 8 * 5);
  _display->setTextColor(SSD1306_BLACK);
  _display->println("Any Button to retry");
  _display->setTextColor(SSD1306_WHITE);
  _display->display();
}
void StateMachine::checkIfPlaying() {
  //check if the player has stopped
  if (this->_musicPlayer->stopped() == true) {
    //check if it was playing something
    if (strcmp(this->_track_playing, "") != 0) {
      this->playNext();
      return;
    }
  }
}
void StateMachine::playNext() {
  //play next track
  if (this->_repeat_mode == 0) {
    if (this->_trackNumber < this->_trackCount - 1) {
      this->_trackNumber++;
      strcpy(this->_track_playing, this->_tracks[_trackNumber]);
      char p[255] = "";
      strcpy(p, this->_root);
      strcat(p, this->_interpret_playing);
      strcat(p, "/");
      strcat(p, this->_album_playing);
      strcat(p, "/");
      strcat(p, this->_track_playing);
      this->_musicPlayer->startPlayingFile(p);
      this->Refresh_Display();
      return;
    }
    else {
      if (strcmp(this->_track_playing, "") != 0) {
        strcpy(this->_interpret_playing, "");
        strcpy(this->_album_playing, "");
        strcpy(this->_track_playing, "");
        this->Refresh_Display();
      }
    }
  }
  //repeat the album
  else if (this->_repeat_mode == 1) {
    this->_trackNumber = (this->_trackNumber + 1) % this->_trackCount;
    strcpy(this->_track_playing, this->_tracks[_trackNumber]);
    char p[255] = "";
    strcpy(p, this->_root);
    strcat(p, this->_interpret_playing);
    strcat(p, "/");
    strcat(p, this->_album_playing);
    strcat(p, "/");
    strcat(p, this->_track_playing);
    this->_musicPlayer->startPlayingFile(p);
    this->Refresh_Display();
    return;
  }
  //repeat the track
  else if (this->_repeat_mode == 2) {
    char p[255] = "";
    strcpy(p, this->_root);
    strcat(p, this->_interpret_playing);
    strcat(p, "/");
    strcat(p, this->_album_playing);
    strcat(p, "/");
    strcat(p, this->_track_playing);
    this->_musicPlayer->startPlayingFile(p);
    this->Refresh_Display();
    return;
  }
  //random
  else if (this->_repeat_mode == 3) {
    this->_trackNumber = random(0, this->_trackCount);
    strcpy(this->_track_playing, this->_tracks[_trackNumber]);
    char p[255] = "";
    strcpy(p, this->_root);
    strcat(p, this->_interpret_playing);
    strcat(p, "/");
    strcat(p, this->_album_playing);
    strcat(p, "/");
    strcat(p, this->_track_playing);
    this->_musicPlayer->startPlayingFile(p);
    this->Refresh_Display();
    return;
  }
}
void StateMachine::playNextTrack() {
  if (this->_trackNumber < this->_trackCount - 1) {
    this->_trackNumber++;
    strcpy(this->_track_playing, this->_tracks[_trackNumber]);
    char p[255] = "";
    strcpy(p, this->_root);
    strcat(p, this->_interpret_playing);
    strcat(p, "/");
    strcat(p, this->_album_playing);
    strcat(p, "/");
    strcat(p, this->_track_playing);
    this->_musicPlayer->startPlayingFile(p);
    this->Refresh_Display();
    return;
  }
}
void StateMachine::playPreviousTrack() {
  if (this->_trackNumber > 0) {
    this->_trackNumber--;
    strcpy(this->_track_playing, this->_tracks[_trackNumber]);
    char p[255] = "";
    strcpy(p, this->_root);
    strcat(p, this->_interpret_playing);
    strcat(p, "/");
    strcat(p, this->_album_playing);
    strcat(p, "/");
    strcat(p, this->_track_playing);
    this->_musicPlayer->startPlayingFile(p);
    this->Refresh_Display();
    return;
  }
}
void StateMachine::ScrollUp_EncRight() {
  _state->ScrollUp_EncRight(this);
}
void StateMachine::ScrollDown_EncRight() {
  _state->ScrollDown_EncRight(this);
}
void StateMachine::ButtonDown_EncRight() {
  _state->ButtonDown_EncRight(this);
}
void StateMachine::ScrollUp_EncLeft() {
  _state->ScrollUp_EncLeft(this);
}
void StateMachine::ScrollDown_EncLeft() {
  _state->ScrollDown_EncLeft(this);
}
void StateMachine::ButtonDown_EncLeft() {
  _state->ButtonDown_EncLeft(this);
}
void StateMachine::Refresh_Display() {
  _state->Refresh_Display(this);
}
void StateMachine::ChangeState(State* s) {
  _state = s;
}
//STATE****************************************
void State::ScrollUp_EncRight(StateMachine*) {}
void State::ScrollDown_EncRight(StateMachine*) {}
void State::ButtonDown_EncRight(StateMachine*) {}
void State::ScrollUp_EncLeft(StateMachine*) {}
void State::ScrollDown_EncLeft(StateMachine*) {}
void State::ButtonDown_EncLeft(StateMachine*) {}
void State::Refresh_Display(StateMachine*) {}
void State::ChangeState(StateMachine* s, State* st) {
  s->ChangeState(st);
}
//STATE INTERPRET*****************************
/*Singleton*/
State_Interprets* State_Interprets::_instance = 0;
State_Interprets* State_Interprets::Instance() {
  if (_instance == 0) {
    _instance = new State_Interprets;
  }
  return _instance;
}
/*UP*/
void State_Interprets::ScrollUp_EncRight(StateMachine* s) {
  if (s->_interpret_screen_item > 0) {
    s->_interpret_screen_item--;
    s->Refresh_Display();
  } else if (s->_interpret_screen_item == 0 && s->_interpret_top_item > 0) {
    s->_interpret_top_item--;
    s->Refresh_Display();
  }
}
/*DOWN*/
void State_Interprets::ScrollDown_EncRight(StateMachine* s) {
  if (s->_interpret_screen_item < min(6, s->_num_items - 1)) {
    s->_interpret_screen_item++;
    s->Refresh_Display();
  } else if (s->_interpret_screen_item == 6 && s->_interpret_top_item < s->_num_items - 7) {
    s->_interpret_top_item++;
    s->Refresh_Display();
  }
}
/*ENTER*/
void State_Interprets::ButtonDown_EncRight(StateMachine* s) {
  //save the selected interpret
  strcpy(s->_interpret, s->_items[s->_interpret_top_item + s->_interpret_screen_item]);
  //get the albums
  char p[255] = "";
  strcpy(p, s->_root);
  strcat(p, s->_interpret); //p is path to the interpret
  strcat(p, "/");
  int count = 0; //count items
  File dir = SD.open(p);
  while (true && count < 255) {
    File entry = dir.openNextFile();
    if (!entry) {
      break;
    }
    //check if ignored
    if (isIgnored(entry.name()) == true) {
      continue;
    }
    //check if folder
    if (entry.isDirectory()) {
      //TODO: skip folders if necessary
      strcpy(s->_items[count], entry.name());
      count++;
    }
    entry.close();
  }
  dir.close();
  s->_num_items = count;
  s->_album_top_item = 0;
  s->_album_screen_item = 0;
  ChangeState(s, State_Albums::Instance());
  s->Refresh_Display();
}
void State_Interprets::ScrollUp_EncLeft(StateMachine*) {}
void State_Interprets::ScrollDown_EncLeft(StateMachine*) {}
/*Back*/
void State_Interprets::ButtonDown_EncLeft(StateMachine* s) {
  ChangeState(s, State_Options::Instance());
  s->Refresh_Display();
}
/*Display*/
void State_Interprets::Refresh_Display(StateMachine* s) {
  //display
  s->_display->clearDisplay();
  s->_display->setCursor(0, 0);
  s->_display->println("Interprets");
  s->_display->drawFastHLine(0, 7, 128, SSD1306_WHITE);
  for (int i = 0; i < min(7, s->_num_items); i++) {
    //println
    s->_display->println(s->_items[s->_interpret_top_item + i]);
    //check if played
    if (strcmp(s->_items[s->_interpret_top_item + i], s->_interpret_playing) == 0) {
      s->_display->fillRect(128 - 8, 8 + 8 * i, 8, (s->_interpret_screen_item == 6) ? 7 : 8, SSD1306_BLACK);
      s->_display->fillCircle(128 - 4, 8 + 8 * i + 4, 2, SSD1306_WHITE);
    }
    //check if selected
    if (i == s->_interpret_screen_item) {
      s->_display->fillRect(0, 8 + 8 * i, 128, (s->_interpret_screen_item == 6) ? 7 : 8, SSD1306_INVERSE);
    }
  }
  s->_display->display();
}
//STATE ALBUMS****************************************
/*Singleton*/
State_Albums* State_Albums::_instance = 0;
State_Albums* State_Albums::Instance() {
  if (_instance == 0) {
    _instance = new State_Albums;
  }
  return _instance;
}
/*Up*/
void State_Albums::ScrollUp_EncRight(StateMachine* s) {
  if (s->_album_screen_item > 0) {
    s->_album_screen_item--;
    s->Refresh_Display();
  } else if (s->_album_screen_item == 0 && s->_album_top_item > 0) {
    s->_album_top_item--;
    s->Refresh_Display();
  }
}
/*Down*/
void State_Albums::ScrollDown_EncRight(StateMachine* s) {
  if (s->_album_screen_item < min(6, s->_num_items - 1)) {
    s->_album_screen_item++;
    s->Refresh_Display();
  } else if (s->_album_screen_item == 6 && s->_album_top_item < s->_num_items - 7) {
    s->_album_top_item++;
    s->Refresh_Display();
  }
}
/*Enter*/
void State_Albums::ButtonDown_EncRight(StateMachine* s) {
  //save the selected album
  strcpy(s->_album, s->_items[s->_album_top_item + s->_album_screen_item]);
  //construct the path to the album
  char p[255] = "";
  strcpy(p, s->_root);
  strcat(p, s->_interpret);
  strcat(p, "/");
  strcat(p, s->_album);
  strcat(p, "/");
  int count = 0;
  File dir = SD.open(p);
  while (true && count < 255) {
    File entry = dir.openNextFile();
    if (!entry) {
      break;
    }
    //check if ignored
    if (isIgnored(entry.name()) == true) {
      continue;
    }
    //check if mp3 file
    char* lastdot = strrchr(entry.name(), '.');
    if (lastdot == NULL) {
      continue;
    }
    if (strcmp(lastdot, ".mp3") == 0) {
      strcpy(s->_items[count], entry.name());
      //strcpy(s->_tracks[count], entry.name());
      count++;
    }
    entry.close();
  }
  dir.close();
  s->_num_items = count;
  //s->_trackCount = count;
  s->_track_top_item = 0;
  s->_track_screen_item = 0;
  ChangeState(s, State_Tracks::Instance());
  s->Refresh_Display();
}
void State_Albums::ScrollUp_EncLeft(StateMachine*) {}
void State_Albums::ScrollDown_EncLeft(StateMachine*) {}
/*Back*/
void State_Albums::ButtonDown_EncLeft(StateMachine* s) {
  char p[255] = "";
  strcpy(p, s->_root);
  int count = 0;
  File dir = SD.open(p);
  while (true && count < 255) {
    File entry = dir.openNextFile();
    if (!entry) {
      break;
    }
    //check if ignored
    if (isIgnored(entry.name()) == true) {
      continue;
    }
    if (entry.isDirectory()) {
      strcpy(s->_items[count], entry.name());
      count++;
    }
    entry.close();
  }
  dir.close();
  s->_num_items = count;
  ChangeState(s, State_Interprets::Instance());
  s->Refresh_Display();
}
/*Display*/
void State_Albums::Refresh_Display(StateMachine* s) {
  s->_display->clearDisplay();
  s->_display->setCursor(0, 0);
  s->_display->println(s->_interpret);
  s->_display->drawFastHLine(0, 7, 128, SSD1306_WHITE);
  for (int i = 0; i < min(7, s->_num_items); i++) {
    //println
    s->_display->println(s->_items[s->_album_top_item + i]);
    //check if played, item matches? interpret matches
    if (strcmp(s->_items[s->_album_top_item + i], s->_album_playing) == 0 && strcmp(s->_interpret, s->_interpret_playing) == 0) {
      s->_display->fillRect(128 - 8, 8 + 8 * i, 8, (s->_album_screen_item == 6) ? 7 : 8, SSD1306_BLACK);
      s->_display->fillCircle(128 - 4, 8 + 8 * i + 4, 2, SSD1306_WHITE);
    }
    //check if selected
    if (i == s->_album_screen_item) {
      s->_display->fillRect(0, 8 + 8 * i, 128, (s->_album_screen_item == 6) ? 7 : 8, SSD1306_INVERSE);
    }
  }
  s->_display->display();
}
//STATE TRACKS**********************************************
/*Singleton*/
State_Tracks* State_Tracks::_instance = 0;
State_Tracks* State_Tracks::Instance() {
  if (_instance == 0) {
    _instance = new State_Tracks;
  }
  return _instance;
}
/*Up*/
void State_Tracks::ScrollUp_EncRight(StateMachine* s) {
  if (s->_track_screen_item > 0) {
    s->_track_screen_item--;
    s->Refresh_Display();
  } else if (s->_track_screen_item == 0 && s->_track_top_item > 0) {
    s->_track_top_item--;
    s->Refresh_Display();
  }
}
/*Down*/
void State_Tracks::ScrollDown_EncRight(StateMachine* s) {
  if (s->_track_screen_item < min(6, s->_num_items - 1)) {
    s->_track_screen_item++;
    s->Refresh_Display();
  } else if (s->_track_screen_item == 6 && s->_track_top_item < s->_num_items - 7) {
    s->_track_top_item++;
    s->Refresh_Display();
  }
}
/*Enter*/
void State_Tracks::ButtonDown_EncRight(StateMachine* s) {
  //check if we are already playing this file
  if (s->_musicPlayer->playingMusic == true && strcmp(s->_interpret, s->_interpret_playing) == 0 && strcmp(s->_album, s->_album_playing) == 0 && strcmp(s->_items[s->_track_top_item + s->_track_screen_item], s->_track_playing) == 0) {
    s->_play_option_item = 1;
    ChangeState(s, State_Playing::Instance());
    s->Refresh_Display();
    return;
  }
  s->_trackCount = s->_num_items;
  for (int i = 0; i < s->_trackCount; i++) {
    strcpy(s->_tracks[i], s->_items[i]);
  }
  strcpy(s->_interpret_playing, s->_interpret);
  strcpy(s->_album_playing, s->_album);
  strcpy(s->_track_playing, s->_items[s->_track_top_item + s->_track_screen_item]);
  s->_trackNumber = s->_track_top_item + s->_track_screen_item;
  char p[255] = "";
  strcpy(p, s->_root);
  strcat(p, s->_interpret_playing);
  strcat(p, "/");
  strcat(p, s->_album_playing);
  strcat(p, "/");
  strcat(p, s->_track_playing);
  s->_musicPlayer->startPlayingFile(p);
  s->_play_option_item = 1;
  ChangeState(s, State_Playing::Instance());
  s->Refresh_Display();
}
void State_Tracks::ScrollUp_EncLeft(StateMachine*) {}
void State_Tracks::ScrollDown_EncLeft(StateMachine*) {}
/*Back*/
void State_Tracks::ButtonDown_EncLeft(StateMachine* s) {
  char p[255] = "";
  strcpy(p, s->_root);
  strcat(p, s->_interpret);
  strcat(p, "/");
  int count = 0;
  File dir = SD.open(p);
  while (true && count < 255) {
    File entry = dir.openNextFile();
    if (!entry) {
      break;
    }
    //check if ignored
    if (isIgnored(entry.name()) == true) {
      continue;
    }
    if (entry.isDirectory()) {
      strcpy(s->_items[count], entry.name());
      count++;
    }
    entry.close();
  }
  dir.close();
  s->_num_items = count;
  ChangeState(s, State_Albums::Instance());
  s->Refresh_Display();
}
/*Display*/
void State_Tracks::Refresh_Display(StateMachine* s) {
  s->_display->clearDisplay();
  s->_display->setCursor(0, 0);
  s->_display->println(s->_album);
  s->_display->drawFastHLine(0, 7, 128, SSD1306_WHITE);
  for (int i = 0; i < min(7, s->_num_items); i++) {
    //println
    //check if truncate
    if(s->_truncateFilename==false){
      s->_display->println(s->_items[s->_track_top_item + i]);
    }else{
      char p[255]="";
      strcpy(p, s->_items[s->_track_top_item+i]);
      char* firstspace = strchr(p,' ');
      strcpy(p,firstspace+1);
      char* last = strstr(p,".mp3");
      char st[255]="";
      strncpy(st,p,(int)(last-&p[0]));
      s->_display->println(st);
    }
    //check if played interpret matches, album matches, item matches
    if (strcmp(s->_items[s->_track_top_item + i], s->_track_playing) == 0 && strcmp(s->_album, s->_album_playing) == 0 && strcmp(s->_interpret, s->_interpret_playing) == 0) {
      s->_display->fillRect(128 - 8, 8 + 8 * i, 8, (s->_track_screen_item == 6) ? 7 : 8, SSD1306_BLACK);
      s->_display->fillCircle(128 - 4, 8 + 8 * i + 4, 2, SSD1306_WHITE);
    }
    //check if selected
    if (i == s->_track_screen_item) {
      s->_display->fillRect(0, 8 + 8 * i, 128, (s->_track_screen_item == 6) ? 7 : 8, SSD1306_INVERSE);
    }
  }
  s->_display->display();
}
//STATE PLAYING*************************************************
/*Singleton*/
State_Playing* State_Playing::_instance = 0;
State_Playing* State_Playing::Instance() {
  if (_instance == 0) {
    _instance = new State_Playing;
  }
  return _instance;
}
void State_Playing::ScrollUp_EncRight(StateMachine*) {}
void State_Playing::ScrollDown_EncRight(StateMachine*) {}
void State_Playing::ButtonDown_EncRight(StateMachine* s) {
  //play previous track
  if (s->_play_option_item == 0) {
    s->playPreviousTrack();
  }
  //pause
  if (s->_play_option_item == 1) {
    s->_musicPlayer->pausePlaying(!s->_musicPlayer->paused());
    s->Refresh_Display();
    return;
  }
  //playNext track
  if (s->_play_option_item == 2) {
    s->playNextTrack();
  }
}

void State_Playing::ScrollUp_EncLeft(StateMachine* s) {
  if (s->_play_option_item < 2) {
    s->_play_option_item++;
    s->Refresh_Display();
  }
}
void State_Playing::ScrollDown_EncLeft(StateMachine* s) {
  if (s->_play_option_item > 0) {
    s->_play_option_item--;
    s->Refresh_Display();
  }
}
/*Back*/
void State_Playing::ButtonDown_EncLeft(StateMachine* s) {
  //construct the path to the album
  char p[255] = "";
  strcpy(p, s->_root);
  strcat(p, s->_interpret);
  strcat(p, "/");
  strcat(p, s->_album);
  strcat(p, "/");
  int count = 0;
  File dir = SD.open(p);
  while (true && count < 255) {
    File entry = dir.openNextFile();
    if (!entry) {
      break;
    }
    //check if ignored
    if (isIgnored(entry.name()) == true) {
      continue;
    }
    //check if mp3 file
    char* lastdot = strrchr(entry.name(), '.');
    if (lastdot == NULL) {
      continue;
    }
    if (strcmp(lastdot, ".mp3") == 0) {
      strcpy(s->_items[count], entry.name());
      count++;
    }
    entry.close();
  }
  dir.close();
  s->_num_items = count;
  ChangeState(s, State_Tracks::Instance());
  s->Refresh_Display();
}
/*Display*/
void State_Playing::Refresh_Display(StateMachine* s) {
  s->_display->clearDisplay();
  s->_display->setCursor(0, 0);
  //line 1
  if (s->_musicPlayer->paused() == true) {
    s->_display->println("PAUSED");
  }
  if (s->_musicPlayer->playingMusic == true) {
    s->_display->println("PLAYING");
  }
  s->_display->println(""); //line 2
  s->_display->println(s->_interpret_playing); //line3
  s->_display->println(s->_album_playing); //line 4
  s->_display->println(s->_track_playing); //line 5
  s->_display->println(""); //line 6
  s->_display->println(""); //line 7
  //line 8
  s->_display->print("<<");
  if (s->_play_option_item == 0) {
    s->_display->fillRect(0, 56, 12, 7, SSD1306_INVERSE);
  }
  for (int i = 0; i < 8; i++) {
    s->_display->print(" ");
  }
  s->_display->print("||");
  if (s->_play_option_item == 1) {
    s->_display->fillRect(59, 56, 13, 7, SSD1306_INVERSE);
  }
  for (int i = 0; i < 7; i++) {
    s->_display->print(" ");
  }
  s->_display->print(">>");
  if (s->_play_option_item == 2) {
    s->_display->fillRect(114, 56, 12, 7, SSD1306_INVERSE);
  }
  s->_display->display();
}
//STATE OPTIONS**************************************************
/*Singleton*/
State_Options* State_Options::_instance = 0;
State_Options* State_Options::Instance() {
  if (_instance == 0) {
    _instance = new State_Options;
  }
  return _instance;
}
/*Up*/
void State_Options::ScrollUp_EncRight(StateMachine* s) {
  if (s->_option_screen_item > 0) {
    s->_option_screen_item --;
    s->Refresh_Display();
  } else if (s->_option_screen_item == 0 && s->_option_top_item > 0) {
    s->_album_top_item--;
    s->Refresh_Display();
  }
}
/*Down*/
void State_Options::ScrollDown_EncRight(StateMachine* s) {
  if (s->_option_screen_item < min(6, s->_num_options - 1)) {
    s->_option_screen_item++;
    s->Refresh_Display();
  } else if (s->_option_screen_item == 6 && s->_option_top_item < s->_num_options - 7) {
    s->_option_top_item++;
    s->Refresh_Display();
  }
}
void State_Options::ButtonDown_EncRight(StateMachine*) {}
/*Left*/
void State_Options::ScrollUp_EncLeft(StateMachine* s) {
  switch (s->_option_top_item + s->_option_screen_item) {
    //Volume
    case 0:
      if (s->_volume > 0) {
        s->_volume-=5;
        s->_musicPlayer->setVolume(255-s->_volume, 255-s->_volume);
        s->Refresh_Display();
      }
      break;
    //Repeat
    case 1:
      if (s->_repeat_mode > 0) {
        s->_repeat_mode--;
        s->Refresh_Display();
      }
      break;
    //dimDisplay
    case 2:
      if(s->_dimDisplay==true){
        s->_dimDisplay=false;
        s->_display->dim(s->_dimDisplay);
        s->Refresh_Display();
      }
      break;
    //truncate filename
    case 3:
      if(s->_truncateFilename==true){
        s->_truncateFilename=false;
        s->Refresh_Display();
      }
      break;
    default: break;
  }
}
/*Right*/
void State_Options::ScrollDown_EncLeft(StateMachine* s) {
  switch (s->_option_top_item + s->_option_screen_item) {
    //Volume
    case 0:
      if (s->_volume < 255) {
        s->_volume+=5;
        s->_musicPlayer->setVolume(255-s->_volume, 255-s->_volume);
        s->Refresh_Display();
      }
      break;
    //Repeat
    case 1:
      if (s->_repeat_mode < 3) {
        s->_repeat_mode++;
        s->Refresh_Display();
      }
      break;
    //dimdisplay
    case 2:
      if(s->_dimDisplay==false){
        s->_dimDisplay = true;
        s->_display->dim(s->_dimDisplay);
        s->Refresh_Display();
      }
      break;
    //truncate filename
    case 3:
      if(s->_truncateFilename==false){
        s->_truncateFilename=true;
        s->Refresh_Display();
      }
      break;
    default: break;
  }
}
/*Back*/
void State_Options::ButtonDown_EncLeft(StateMachine* s) {
  int count = 0;
  File dir = SD.open(s->_root);
  while (true && count < 255) {
    File entry = dir.openNextFile();
    if (!entry) {
      break;
    }
    //check if ignored
    if (isIgnored(entry.name()) == true) {
      continue;
    }
    //check if folder
    if (entry.isDirectory()) {
      strcpy(s->_items[count], entry.name());
      count++;
    }
    entry.close();
  }
  dir.close();
  s->_num_items = count;
  s->_interpret_top_item = 0;
  s->_interpret_screen_item = 0;
  ChangeState(s, State_Interprets::Instance());
  s->Refresh_Display();
}
/*Display*/
void State_Options::Refresh_Display(StateMachine* s) {
  s->_display->clearDisplay();
  s->_display->setCursor(0, 0);
  s->_display->println("Options");
  s->_display->drawFastHLine(0, 7, 128, SSD1306_WHITE);
  for (int i = 0; i < min(7, s->_num_options); i++) {
    char o[255] = "";
    strcpy(o, s->_options[s->_option_top_item + i]);
    switch (s->_option_top_item + i) {
      //Volume
      case 0:
        strcat(o, ": ");
        char cstr[2];
        itoa(s->_volume, cstr, 10);
        strcat(o, cstr);
        s->_display->drawRect(68,8+8*i+1,126-68,6, SSD1306_WHITE);
        //int percentage = ;
        s->_display->fillRect(68,8+8*i+1,(int)((float)s->_volume/255*(126-68)),6,SSD1306_WHITE);
        break;
      //Repeat
      case 1:
        strcat(o, ": ");
        switch (s->_repeat_mode) {
          case 0: strcat(o, "None"); break;
          case 1: strcat(o, "Repeat Album"); break;
          case 2: strcat(o, "Repeat Track"); break;
          case 3: strcat(o, "Random");break;
          default: strcat(o, "-"); break;
        }
        break;
      //dimdisplay
      case 2:
        strcat(o,": ");
        if(s->_dimDisplay){
          strcat(o, "True");
        }else{
          strcat(o,"False");
        }
        break;
      //truncate filename
      case 3:
        strcat(o, ": ");
        if(s->_truncateFilename==false){
          strcat(o, "Full");
        }else{
          strcat(o, "Truncate");
        }
        break;
      default:
        break;
    }
    s->_display->println(o);
    if(i== s->_option_screen_item){
      s->_display->fillRect(0, 8 + 8 * i, 127, (s->_option_screen_item == 6) ? 7 : 8, SSD1306_INVERSE);
    }
  }
  s->_display->display();
}
