#ifndef context_h
#define context_h

#include <lcd.h>
#include <EEPROM.h>
#include "Arduino.h"

#define PIN_LCD_CLK 6
#define PIN_LCD_DIN 7
#define PIN_LCD_DC  8
#define PIN_LCD_RST 9
#define PIN_LCD_CE  10

#define PIN_BUZZER 12

/*Event represent input and processed by Context*/
#define EVENT_MODE_BT  0
#define EVENT_ROLL     1
#define EVENT_ROLL_BT  2

class Event {
  public:
    Event(int source, int value) : source(source), value(value) {};
    const int getSource() {return source;}
    int getValue() {return value;}
    void setValue(int value) {this->value = value;}
  private:
    const int source;
    int value;
};

/*Base mode class. Context has bunch of modes*/
#define MODE_TIMER   0
#define MODE_SETTING 1

class Mode {
  public:
    virtual void loop();
    virtual void processEvent(Event *event);
    virtual void activate();
    virtual void deactivate();
};

/*Configurable system setting*/
#define SETTING_MIN_SPLIT     0
#define SETTING_MIC_THRESHOLD 1
#define SETTING_START_DELAY   2
#define SETTING_LIGHT_DELAY   3
#define SETTING_BUZZER_DELAY  4
#define SETTING_DEBUG_SENSOR  5

class Setting {
  public:
    Setting(char *name, int value, int step, int minVal, int maxVal) : name(name), value(value), step(step), maxVal(maxVal), minVal(minVal) {
      //don't allow invalid value
      if(value < minVal || value > maxVal) {
        this->value = minVal;
      } 
    }
    void up() {
      if(value <= maxVal-step) {
        value += step;
      }
    }
    void down() {
      if(value >= minVal+step) {
        value -= step;
      }
    }
    int getValue() {return value;}
    char *getName() {return name;}
  private:
    char *name;
    int value;
    const int step;
    const int minVal;
    const int maxVal;
};

/*Main application context. If initiated and called by main function.
Context provides Modes management, and forwards loop() and processEvent() calls
to the active one. Also Context contains resources and provides access to them 
(Settings, LCD Screen, Buzzer)*/
class Context {
  public:
   const int settingCapacity = 6;
   const int modeCapacity = 2;
   Context();
   void processEvent(Event *event);
   void loop() {getActiveMode()->loop();}
   //resources
   Setting **settings;
   Lcd *lcd;
   void beep();
   int getSetting(int name) {return settings[name]->getValue();}
   void upSetting(int name);
   void downSetting(int name);
  private:
   Mode *getActiveMode() {return modes[activeMode];}
   void initSetting(int name, char *label, int step, int min, int max);
   void writeSetting(int name);
   Mode **modes;
   int activeMode = 0;
};

#endif
