#include "timer.h"

extern uint8_t SmallFont[];
extern uint8_t TinyFont[];
extern uint8_t MediumNumbers[];
extern uint8_t BigNumbers[];

void TimerMode::loop() {
  switch (mode) {
    case MODE_SET:
      {
        // button clicked, goint to start soon
        long now = millis();
        if((now - start_time) >= context->getSetting(SETTING_START_DELAY)*1000) {
          // time to start
          start_time = millis();
          mode = MODE_GO;      
          shot = FIRST_SHOT;
          context->beep();
          print(shot);
        }
        break;
      }
    case MODE_GO:
      {
        // active listening
        sensorVal = analogRead(SENSOR_PIN);
        if(context->getSetting(SETTING_DEBUG_SENSOR)) {
          Serial.println(sensorVal);
        }
        if(sensorVal > lastSensorVal + context->getSetting(SETTING_MIC_THRESHOLD)) { 
          boom(millis());
        }
        lastSensorVal = sensorVal;
        if(sensorVal > maxSensorVal) {
          maxSensorVal = sensorVal;
        }
        break;
      }
  }
}

void TimerMode::processEvent(Event *event) {
  switch(event->getSource()){
    case EVENT_ROLL:
      if(shot != FIRST_SHOT) {
        if(event->getValue() > 0) {
          if(current_shot >= shot) {
            current_shot = 0;
          } else {
            current_shot++;
          }
        } else if (event->getValue() < 0){
          if(current_shot <= 0) {
            current_shot = shot;
          } else {
            current_shot--;
          }
        }
      }
      break;
    case EVENT_ROLL_BT:
      switch (mode) {
        case MODE_READY:
          start_time = millis();
          mode = MODE_SET;
          current_shot = FIRST_SHOT;
          maxSensorVal = 0;
          break;
        case MODE_SET:
        case MODE_GO:
          mode = MODE_READY;
          break;
      }
      break;
  }
  print(current_shot);
}

void TimerMode::boom(long shot_time) {
  // get split between current shot_time and last shot
  long split = shot_time - getPrev(shot+1);
  if(split >= context->getSetting(SETTING_MIN_SPLIT)*10) {
    // split is fine, save shot
    if(shot == max_shots-1) {
      // max shots reached, rollout
      shot = FIRST_SHOT;
    }    
    shot++;
    shots[shot] = shot_time;
    current_shot = shot;
  }
  print(shot);
}

long TimerMode::getPrev(int shot) {
  // calculate split
  long prev;
  if(shot == FIRST_SHOT || shot == 0) {
    prev = start_time;
  } else {
    prev = shots[shot-1];
  }
  return prev;
}

void TimerMode::activate() {
  print(shot);
}

void TimerMode::print(int shot) {
  context->lcd->clrScr();

  //titles
  context->lcd->setFont(TinyFont);   
  context->lcd->print("Shots:", 2, 4);
  context->lcd->print("First:", 2, 12);
  context->lcd->print("Split:", 2, 20);
  if(context->getSetting(SETTING_DEBUG_SENSOR)) {
    context->lcd->printNumI(maxSensorVal, 62, 2);
  }
  switch (mode) {
    case MODE_READY:
      context->lcd->invArea(78, 1, 5, 5);
      break;
    case MODE_SET:
      context->lcd->drawLine(78, 1, 83, 3);
      context->lcd->drawLine(83, 3, 78, 5);
      context->lcd->drawLine(78, 5, 78, 1);
      break;
    case MODE_GO:
      context->lcd->invArea(79, 2, 3, 3);
      context->lcd->drawLine(78, 1, 83, 3);
      context->lcd->drawLine(83, 3, 78, 5);
      context->lcd->drawLine(78, 5, 78, 1);
      break;
  }
  context->lcd->setFont(SmallFont);   
  if(shot != FIRST_SHOT) {
    // shots string
    String s = "";
    s += (shot+1);
    s += "/";
    s += (this->shot+1);
    context->lcd->printStr(s, 30, 2);
    // first shot
    s = "";
    s += ((shots[0] - start_time)/1000.);
    context->lcd->printStr(s, 30, 10);
    // split
    s = "";
    s += ((shots[shot] - getPrev(shot))/1000.);
    context->lcd->printStr(s, 30, 18);
    // total
    context->lcd->setFont(MediumNumbers);   
    s = "";
    s += ((shots[shot] - start_time)/1000.);
    context->lcd->printStr(s, 2, 28);
  }
  context->lcd->setFont(TinyFont);
  
  int batt = analogRead(BATT_PIN);
  double b = ((double)batt * 5.39 )/1023;
  context->lcd->printBatt(60, 41, b);

  context->lcd->update();
}

