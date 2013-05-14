#ifndef SEARCH_H
#define SEARCH_H

#define RIGHT 1
#define LEFT -1

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
  }
}

#endif