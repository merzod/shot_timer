#include "context.h"
#include "timer.h"
#include "settings.h"

extern uint8_t SmallFont[];
extern uint8_t TinyFont[];
extern uint8_t MediumNumbers[];
extern uint8_t BigNumbers[];

Context::Context() {
  lcd = new Lcd(PIN_LCD_CLK, PIN_LCD_DIN, PIN_LCD_DC, PIN_LCD_RST, PIN_LCD_CE);
  lcd->InitLCD(); 
  lcd->setFont(TinyFont); 
  
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);
  
  modes = new Mode *[modeCapacity];
  modes[MODE_TIMER] =   new TimerMode(this);
  modes[MODE_SETTING] = new SettingMode(this);
  
  settings = new Setting *[settingCapacity];
  initSetting(SETTING_MIN_SPLIT,     "MIN_SPLIT/100", 1, 2, 100);
  initSetting(SETTING_MIC_THRESHOLD, "MIC_THRESHOLD", 1, 3, 1000);
  initSetting(SETTING_START_DELAY,   "START_DELAY(s)", 1, 0, 30);
  initSetting(SETTING_LIGHT_DELAY,   "LIGHT_DELAY(s)", 1, 0, 30);
  initSetting(SETTING_BUZZER_DELAY,  "BUZZER_DELAY/10", 1, 0, 10);
  initSetting(SETTING_DEBUG_SENSOR,  "DEBUG_SENSOR", 1, 0, 1);
    
  getActiveMode()->activate();
}

void Context::beep() {
  int del = getSetting(SETTING_BUZZER_DELAY)*100; 
  if(del != 0) {
    digitalWrite(PIN_BUZZER, HIGH);
    delay(del);
    digitalWrite(PIN_BUZZER, LOW);
  }
}

void Context::initSetting(int name, char *label, int step, int min, int max) {
  byte memVal = EEPROM.read(name);
  settings[name] = new Setting(label, memVal, step, min, max);
}

void Context::processEvent(Event *event) {
  switch(event->getSource()){
    case EVENT_MODE_BT:
      getActiveMode()->deactivate();  
      if(activeMode == modeCapacity - 1) {
        activeMode = 0;
      } else {
        activeMode++;
      }
      getActiveMode()->activate();
      break;
    default:
      getActiveMode()->processEvent(event);
  }
  delete(event);
}

void Context::upSetting(int name) {
  settings[name]->up();
  writeSetting(name);
}

void Context::downSetting(int name) {
  settings[name]->down();
  writeSetting(name);
}

void Context::writeSetting(int name) {
  EEPROM.write(name, getSetting(name));
}
