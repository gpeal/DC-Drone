#ifndef SEARCH_H
#define SEARCH_H


#include "MotorDriver.h"
#include "Tracker.h"

namespace StateMachine
{
  namespace Searching
  {
    void enter(void);
    void loop(void);
    void spin(int duty, int direction);
    extern MotorDriver *motor_driver;
    extern Tracker *tracker;
    extern int last_non_none_state;
    extern long last_hit_millis;
    extern const int RIGHT;
    extern const int LEFT;
  }
}

#endif