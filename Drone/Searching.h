#ifndef SEARCH_H
#define SEARCH_H

#include "MotorDriver.h"
#include "Tracker.h"

namespace StateMachine
{
  namespace Searching
  {
    void loop(void);
    extern MotorDriver *motor_driver;
    extern Metro *search_timer;
    extern Tracker *tracker;
    extern int search_count;
  }
}

#endif