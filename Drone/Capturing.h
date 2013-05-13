#ifndef CAPTURING_H
#define CAPTURING_H
/*
 * The StateMachine::Capturing namespace should handle all logic specific to the CAPTURING state
 * There is a loop function that is expected to be run every iteration of the main loop as long
 * as the state is CAPTURING
 */

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