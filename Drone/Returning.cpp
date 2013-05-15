#include "Attacking.h"
#include "Motor.h"
#include "Returning.h"
#include "Sonar.h"
#include "StateMachine.h"
#include "Utils.h"

int StateMachine::Returning::hit_count = 0;
int StateMachine::Returning::miss_count = 0;
int StateMachine::Returning::state;
int StateMachine::Returning::last_non_none_state;
Tracker *StateMachine::Returning::tracker;
MotorDriver *StateMachine::Returning::motor_driver;
Metro *StateMachine::Returning::search_timer = new Metro(10, 1);
int StateMachine::Returning::search_count;
long StateMachine::Returning::skipping_millis;
long StateMachine::Returning::start_search_millis;

/**
 * This is automatically called when the state is entered
 */
void StateMachine::Returning::enter(void)
{
  state = RETURNING_STATE_SEARCHING;
  last_non_none_state = TRACKER_STATE_LEFT;
  skipping_millis = 0;
  search_count = 0;
  hit_count = 0;
  miss_count = 0;
  start_search_millis = millis();
}

void StateMachine::Returning::loop(void)
{
  if (tracker->loop())
  {
    Sonar::loop();
  }
  else
  {
    return;
  }

  switch (state)
  {
    case RETURNING_STATE_SEARCHING:
      if (tracker->state == TRACKER_STATE_NONE)
      {
        search();
      }
      else
      {
        state = RETURNING_STATE_MEASURING;
        miss_count = 0;
        hit_count = 0;
        motor_driver->set(0, 0);
      }
      break;
    case RETURNING_STATE_MEASURING:
      measure();
      break;
    case RETURNING_STATE_SKIPPING:
      skip();
      break;
    case RETURNING_STATE_RETURNING:
      drive();
      break;
  }

  if (tracker->state != TRACKER_STATE_NONE && state != RETURNING_STATE_SKIPPING)
  {
    last_non_none_state = tracker->state;
  }
}

/**
 * Spin in the direction of the last hit to try to find the nest again
 */
void StateMachine::Returning::search()
{
  int min_ticks = 4;
  int max_ticks = 30;
  int ramp_up_time = 5;
  int ticks_to_skip =  (float)(millis() - start_search_millis) / 1000.0 * -(float)(max_ticks - min_ticks) / (float)ramp_up_time + max_ticks;
  ticks_to_skip = cap(ticks_to_skip, min_ticks, max_ticks);

  if (search_timer->check())
  {
    search_count++;
    if (search_count % ticks_to_skip == 0)
    {
      // the left laser hit in the last non none state
      // turn left
      if (last_non_none_state >> 2 == 1)
      {
        motor_driver->set(-255, 255);
      }
      else
      {
        motor_driver->set(255, -255);
      }
    }
    else
    {
      motor_driver->set(0, 0);
    }
  }
}

/*
 * spin to avoid the retroreflective tape that isn't the nest
 */
void StateMachine::Returning::skip(void)
{
  if (millis() - skipping_millis > 1000)
  {
    state = RETURNING_STATE_SEARCHING;
  }
}

void StateMachine::Returning::measure(void)
{
  if (tracker->state == TRACKER_STATE_NONE)
  {
    miss_count++;
  }
  else
  {
    hit_count++;
  }
  // if enough data has been collected, determine whether or not it is the home base
  // and drive towards it or skip it
  if (miss_count + hit_count > 250)
  {
    // less than 80% of the time it was hitting the tape
    // that means it was probably moving
    if ((float)hit_count / (float)(miss_count + hit_count) < 0.8)
    {
      state = RETURNING_STATE_RETURNING;
    }
    // The tape didn't move. Probably not the nest
    else
    {
      skipping_millis = millis();
      state = RETURNING_STATE_SKIPPING;
      // the left laser hit in the last non none state
      // turn left
      if (last_non_none_state & 4 == 1)
      {
        motor_driver->set(255, -255);
      }
      else
      {
        motor_driver->set(-255, 255);
      }
    }
    miss_count = 0;
    hit_count = 0;
  }
}

void StateMachine::Returning::drive(void)
{
  switch (tracker->state)
  {
    case TRACKER_STATE_NONE:
      search();
      break;
    case TRACKER_STATE_LEFT:
      motor_driver->set(245, 255);
      start_search_millis = millis();
      last_non_none_state = tracker->state;
      break;
    case TRACKER_STATE_RIGHT:
      motor_driver->set(255, 245);
      start_search_millis = millis();
      last_non_none_state = tracker->state;
      break;
    case TRACKER_STATE_LEFT_RIGHT:
      motor_driver->set(255, 255);
      start_search_millis = millis();
      last_non_none_state = tracker->state;
      break;
  }
}