#ifndef timer_h
#define timer_h

#include "context.h"
#include "Arduino.h"

#define SENSOR_PIN       A0
#define BATT_PIN         A1
#define FIRST_SHOT       -1

#define MODE_READY 0
#define MODE_SET   1
#define MODE_GO    2

/*Main mode.
Works in own 3 modes:
MODE_READY means that no activity occures, used on start and iterating over results
MODE_SET button pressed and we are about to start round, in some delay buzzer signal
MODE_GO listening mic in order to find shot, store it and calculate all the stuff 
(first, split etc.)*/
class TimerMode : public Mode {
  public:
    TimerMode(Context *context) : context(context) {shots = new long[max_shots];}
    virtual void loop();
    virtual void processEvent(Event *event);
    virtual void activate();
    virtual void deactivate() {}   
  private:
    Context *context;
    // other attributes
    const int max_shots = 50;
    int sensorVal = 0;
    int lastSensorVal = 1000;
    long *shots;
    int shot = FIRST_SHOT;
    long start_time = 0;
    int current_shot = FIRST_SHOT;
    int mode = MODE_READY;
    int maxSensorVal = 0;
    
    void boom(long shot_time);
    void print(int shot);
    long getPrev(int shot);
};

#endif
