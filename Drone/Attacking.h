#ifndef ATTACH_H
#define ATTACH_H

#include "MotorDriver.h"
#include "Tracker.h"

namespace StateMachine
{
  namespace Attacking
  {
    void loop(void);
    extern MotorDriver *motor_driver;
    extern Tracker *tracker;
  }
}

#endif