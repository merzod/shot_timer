#define LOOP_TIME        10
// pins
#define ROLL_BT_PIN    5
#define MENU_BT_PIN      2
#define ENC_A_PIN        3
#define ENC_B_PIN        4
#define LIGHT_PIN        11

#include <lcd.h>
#include <LCD5110_Graph.h>
#include <EEPROM.h>
#include "context.h"

Context *context;
int oldBt = 1;
int oldEncA = 0;
int encVal = 0;
long light_start = 0;
int lightVal = 0;

void setup () {
  Serial.begin (9600);
  pinMode(ROLL_BT_PIN, INPUT_PULLUP);
  pinMode(MENU_BT_PIN, INPUT_PULLUP);
  pinMode(ENC_A_PIN, INPUT_PULLUP);
  pinMode(ENC_B_PIN, INPUT_PULLUP);
  pinMode(LIGHT_PIN, OUTPUT);

  attachInterrupt(0, menuButton, CHANGE);
  attachInterrupt(1, encoderRoll, CHANGE);
  context = new Context();
}
 
void loop () {
  // check light
  light();
  
  // check for roll button. both interrupt pins are occupated :(
  int bt = digitalRead(ROLL_BT_PIN);
  if(bt && !oldBt) { // if button released
    fireEvent(EVENT_ROLL_BT, 0);
  }
  oldBt = bt;
  
  // let Context do it's job
  context->loop();
  delay(LOOP_TIME);
}

/*Check light_start time and either start or stop light. Called on each loop*/
void light() {
  long now = millis();
  int oldLightVal = lightVal;
  if((now - light_start) >= context->getSetting(SETTING_LIGHT_DELAY)*1000) {
    lightVal = LOW;
  } else {
    lightVal = HIGH;
  }
  if(oldLightVal != lightVal) {
    digitalWrite(LIGHT_PIN, lightVal);
  }
}

/*Menu button pressed*/
void menuButton() {
  if(digitalRead(MENU_BT_PIN)) {
    fireEvent(EVENT_MODE_BT, 0);
  }
}

/*Encoder rolled*/
void encoderRoll() {
  int encA = digitalRead(ENC_A_PIN);
  int encB = digitalRead(ENC_B_PIN);
  
  if((!encA) && oldEncA) {
    if(encB) {
      fireEvent(EVENT_ROLL, -1);
    } else {
      fireEvent(EVENT_ROLL, 1);
    }
  }
  oldEncA = encA;
}

/*Create Event and let context process it, update light_start time*/
void fireEvent(int source, int value) {
  light_start = millis();
  Event *e = new Event(source, value);
  context->processEvent(e);
}
