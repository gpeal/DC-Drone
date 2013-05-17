#ifndef RELOCATING_H
#define RELOCATING_H

#include "MotorDriver.h"

#define THRESHOLD_DISTANCE 40
#define RELOCATED_DISTANCE 30
#define MAX_RELOCATION_TIME 5000
#define relocating() relocating_start_time != 0

namespace StateMachine
{
  namespace Relocating
  {
    void enter(void);
    void loop(void);
    extern MotorDriver *motor_driver;
    extern long relocating_start_time;
    extern long spinning_start_time;
  }
}


#endif