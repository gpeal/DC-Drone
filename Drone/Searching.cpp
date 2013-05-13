#include <Arduino.h>
#include "Searching.h"
#include "StateMachine.h"
#include "Utils.h"
/*
 * The StateMachine::Searching namespace should handle all logic specific to the SEARCHING state
 * There is a loop function that is expected to be run every iteration of the main loop as long
 * as the state is SEARCHING
 */

Metro *StateMachine::Searching::search_timer = new Metro(10, 1);
int StateMachine::Searching::search_count = 0;
MotorDriver *StateMachine::Searching::motor_driver;
Tracker *StateMachine::Searching::tracker;

void StateMachine::Searching::loop(void)
{
  int min_ticks;
  int max_ticks;
  int ramp_up_time;
  int ticks_to_skip;

  tracker->loop();
  if (tracker->state != TRACKER_STATE_NONE)
  {
    enter(ATTACKING);
    return;
  }

  min_ticks = 4;
  max_ticks = 30;
  ramp_up_time = 5;
  ticks_to_skip =  (float)(millis() - enter_millis) / 1000.0 * -(float)(max_ticks - min_ticks) / (float)ramp_up_time + max_ticks;
  ticks_to_skip = cap(ticks_to_skip, min_ticks, max_ticks);

  if (search_timer->check())
  {
    search_count++;
    if (search_count % ticks_to_skip == 0)
    {
      motor_driver->set(255, -255);
    }
    else
    {
      motor_driver->set(0, 0);
    }
  }
}