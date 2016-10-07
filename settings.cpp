#include "settings.h"

SettingMode::SettingMode(Context *context) {
  this->context = context;
}

void SettingMode::processEvent(Event *event) {
  switch(event->getSource()){
    case EVENT_ROLL:
      if(mode == MODE_SELECT) {
        rotate(event->getValue());
      } else if(mode == MODE_EDIT) {
        edit(event->getValue());
      }
      print();
      break;
    case EVENT_ROLL_BT:
      mode = !mode; // switch modes (MODE_SELECT, MODE_EDIT)
      print();
      break;
  }
}

void SettingMode::rotate(int event_val) {
  if(event_val > 0) {  
    if(active >= context->settingCapacity - 1) {
      active = 0;
    } else {
      active++;
    }
  } else {
    if(active <= 0) {
      active = context->settingCapacity - 1;
    } else {
      active--;
    }
  }
}

void SettingMode::edit(int event_val) {
  if(event_val > 0) {  
    context->upSetting(active);
  } else {
    context->downSetting(active);  
  }
}

void SettingMode::activate() {
  print();
}

void SettingMode::print() {
  Serial.print("Active: ");
  Serial.println(active);
  context->lcd->clrScr();
  for(int i=0; i<context->settingCapacity; i++) {
    context->lcd->print(context->settings[i]->getName(), 3, 2+i*8);
    context->lcd->printNumI(context->settings[i]->getValue(), 70, 2+i*8);
    if(i == active) {
      if(mode == MODE_SELECT) {
        context->lcd->drawRect(1, i*8, 83, 8+i*8);
      } else if(mode == MODE_EDIT) {
        context->lcd->invArea(1, i*8, 83, 9);
      }
    }
  }
  context->lcd->update();
}
