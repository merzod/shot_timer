#ifndef setting_h
#define setting_h

#include "context.h"
#include "Arduino.h"

#define MODE_SELECT 0
#define MODE_EDIT 1

/*Provides Context's settings management.
Has 2 modes:
MODE_SELECT for selecting setting
MODE_EDIT for editing selected setting*/
class SettingMode : public Mode {
  public:
    SettingMode(Context *context);
    virtual void loop() {}
    virtual void processEvent(Event *event);
    virtual void activate();
    virtual void deactivate() {}
  private:
    Context *context;
    int active = 0;
    int mode = MODE_SELECT;
    void rotate(int event_value);
    void edit(int event_value);
    void print();
};

#endif
