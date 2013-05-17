#ifndef RELOCATING_H
#define RELOCATING_H

#include "MotorDriver.h"

#define THRESHOLD_DISTANCE 30
#define relocating() relocating_start_time != 0

namespace StateMachine
{
  namespace Relocating
  {
    void enter(void);
    void loop(void);
    extern MotorDriver *motor_driver;
    extern long relocating_start_time;
  }
}


#endif