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
    void track(void);
    void spin(void);
    extern MotorDriver *motor_driver;
    extern Metro *search_timer;
    extern Tracker *tracker;
    extern int spin_count;
    extern int spin_state;
    extern long spin_micros;
    extern int track_count;
    extern int last_non_none_state;
  }
}

#endif