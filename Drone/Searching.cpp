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
long StateMachine::Searching::spin_micros;
int StateMachine::Searching::spin_count = 0;
int StateMachine::Searching::spin_state = 0;
int StateMachine::Searching::track_count = 0;
MotorDriver *StateMachine::Searching::motor_driver;
Tracker *StateMachine::Searching::tracker;
int StateMachine::Searching::last_non_none_state = TRACKER_STATE_LEFT;

void StateMachine::Searching::enter(void)
{
  spin_micros = micros();
}

void StateMachine::Searching::loop(void)
{
  tracker->loop();
  if (tracker->state == TRACKER_STATE_NONE)
  {
    spin();
  }
  else
  {
    spin();
    // track();
  }
}

void StateMachine::Searching::track(void)
{
  int ticks_to_skip = 4;
  // if the middle laser hits, move slower
  if (tracker->state >> 1 & 1 == 1)
  {
    ticks_to_skip *= 2;
  }

  if (search_timer->check())
  {
    track_count++;
    if (track_count % ticks_to_skip == 0)
    {
      // right laser hit
      if (tracker->state & 1 == 1)
      {
        motor_driver->set(255, -255);
      }
      // left laser hit
      else if (tracker->state >> 2 & 1 == 1)
      {
        motor_driver->set(-255, 255);
      }
    }
    else
    {
      motor_driver->set(0, 0);
    }
  }
  spin_micros = micros();
}

void StateMachine::Searching::spin(void)
{
  int direction;
  int min_ticks = 10000;
  int max_ticks = 30000;
  int ramp_up_time = 5;
  int ticks_to_skip; // =  (float)(millis() - spin_micros) / 1000.0 * -(float)(max_ticks - min_ticks) / (float)ramp_up_time + max_ticks;
  // ticks_to_skip = cap(ticks_to_skip, min_ticks, max_ticks)
  if (millis() - spin_micros < ramp_up_time * 1000000)
  {
    ticks_to_skip = max_ticks;
  }
  else
  {
    ticks_to_skip = min_ticks;
  }

  // if the right laser hit in the last non none state, turn right
  if (last_non_none_state & 1 == 1)
  {
    direction = 1;
  }
  else
  {
    direction = -1;
  }
  if (micros() - spin_micros > ticks_to_skip)
  {
    spin_state = !spin_state;
    spin_micros = micros();
  }
  else
  {
    motor_driver->set(255 * direction * spin_state, 255 * -direction * spin_state);
  }
}