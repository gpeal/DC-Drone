#include <Arduino.h>
#include "Searching.h"
#include "StateMachine.h"
#include "Utils.h"
/*
 * The StateMachine::Searching namespace should handle all logic specific to the SEARCHING state
 * There is a loop function that is expected to be run every iteration of the main loop as long
 * as the state is SEARCHING
 */

MotorDriver *StateMachine::Searching::motor_driver;
Tracker *StateMachine::Searching::tracker;
int StateMachine::Searching::last_non_none_state = TRACKER_STATE_NONE;


void StateMachine::Searching::enter(void)
{
}


void StateMachine::Searching::loop(void)
{
  int min_duty;
  int max_duty;
  int ramp_up_time;
  int duty;

  tracker->loop();

  if (tracker->state != TRACKER_STATE_NONE)
  {
    last_non_none_state = tracker->state;
    enter(ATTACKING);
  }

  // the tracker has hit something before, just track it
  if (last_non_none_state != TRACKER_STATE_NONE)
  {
    switch(tracker->state)
    {
      case TRACKER_STATE_LEFT:
        motor_driver->spin(15, LEFT);
        break;
      case TRACKER_STATE_LEFT_MIDDLE:
        motor_driver->spin(10, LEFT);
        break;
      case TRACKER_STATE_MIDDLE_RIGHT:
        motor_driver->spin(10, RIGHT);
        break;
      case TRACKER_STATE_RIGHT:
        motor_driver->spin(15, RIGHT);
        break;
      case TRACKER_STATE_NONE:
        // the right laser hit in the last non none state
        if (last_non_none_state & 1 == 1)
        {
          motor_driver->spin(15, RIGHT);
        }
        else
        {
          motor_driver->spin(15, LEFT);
        }
        break;
      default:
        motor_driver->set(0, 0);
        break;
    }
  }
  else
  {
    min_duty = 1;
    max_duty = 13;
    ramp_up_time = 5;
    duty =  (float)(millis() - enter_millis) / 1000.0 * (float)(max_duty - min_duty) / (float)ramp_up_time + min_duty;
    duty = cap(duty, min_duty, max_duty);
    motor_driver->spin(duty, RIGHT);
  }



}