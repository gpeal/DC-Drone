#ifndef CAPTURING_H
#define CAPTURING_H

#include <Metro.h>
#include "MotorDriver.h"

namespace StateMachine
{
  namespace Capturing
  {
    void loop(void);
    extern MotorDriver *motor_driver;
    extern Metro *timer;
    extern int state;
  }
}



#endif