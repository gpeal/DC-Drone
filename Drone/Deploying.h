#ifndef DEPLOYING_H
#define DEPLOYING_H

#include "MotorDriver.h"

namespace StateMachine
{
  namespace Deploying
  {
    void enter(void);
    void loop(void);
    extern MotorDriver *motor_driver;
    extern bool complete;
  }
}

#endif