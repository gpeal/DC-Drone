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
  }

  // the tracker has hit something before, just track it
  if (last_non_none_state != TRACKER_STATE_NONE)
  {
    switch(tracker->state)
    {
      case TRACKER_STATE_LEFT:
        spin(80, LEFT);
        break;
      case TRACKER_STATE_LEFT_MIDDLE:
        spin(10, LEFT);
        break;
      case TRACKER_STATE_MIDDLE_RIGHT:
        spin(10, RIGHT);
        break;
      case TRACKER_STATE_RIGHT:
        spin(80, RIGHT);
        break;
      case TRACKER_STATE_NONE:
        // the right laser hit in the last non none state
        if (last_non_none_state & 1 == 1)
        {
          spin(30, RIGHT);
        }
        else
        {
          spin(30, LEFT);
        }
        break;
    }
  }
  else
  {
    min_duty = 1;
    max_duty = 10;
    ramp_up_time = 5;
    duty =  (float)(millis() - enter_millis) / 1000.0 * (float)(max_duty - min_duty) / (float)ramp_up_time + min_duty;
    duty = cap(duty, min_duty, max_duty);
    spin(duty, RIGHT);
  }



}

/**
 * Turn the motors on for duty millis every 100 millis
 * Direction takes RIGHT or LEFT
 */
void StateMachine::Searching::spin(int duty, int direction)
{
  if ((millis() - enter_millis) % 100 < duty)
  {
    motor_driver->set(direction * 255,  -direction * 255);
  }
  else
  {
    motor_driver->set(0, 0);
  }
}