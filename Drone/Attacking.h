#ifndef ATTACK_H
#define ATTACK_H
/*
 * The StateMachine::Attack namespace should handle all logic specific to the ATTACKING state
 * There is a loop function that is expected to be run every iteration of the main loop as long
 * as the state is ATTACKING
 */

#include "MotorDriver.h"
#include "Tracker.h"

namespace StateMachine
{
  namespace Attacking
  {
    void enter(void);
    void loop(void);
    void spin(int duty, int direction);
    extern MotorDriver *motor_driver;
    extern Tracker *tracker;
    extern int last_non_none_state;
    extern long last_hit_millis;
    extern int captured;
    extern const int RIGHT;
    extern const int LEFT;
  }
}

#endif