#include "Motor.h"
#include "Returning.h"

int StateMachine::Returning::hit_count = 0;
int StateMachine::Returning::miss_count = 0;
int StateMachine::Returning::state = RETURNING_STATE_SEARCHING;
int StateMachine::Returning::last_non_none_state = TRACKER_STATE_LEFT;
Tracker *StateMachine::Returning::tracker;
MotorDriver *StateMachine::Returning::motor_driver;
int StateMachine::Returning::search_count = 0;

/**
 * This is automatically called when the state is entered
 */
void StateMachine::Returning::enter(void)
{
  hit_count = 0;
  miss_count = 0;
}

void StateMachine::Returning::loop(void)
{
  tracker->loop();

  if (tracker->state == TRACKER_STATE_NONE && state != RETURNING_STATE_MEASURING)
  {
    search();
  }

  switch (state)
  {
    case RETURNING_STATE_SEARCHING
      if (tracker->state == TRACKER_STATE_NONE)
      {
        spin();
      }
      else
      {
        state = RETURNING_STATE_MEASURING;
      }
      break;
    case RETURNING_STATE_MEASURING:
      measure();
      break;
    case RETURNING_STATE_RETURNING:
      drive();
      break;
  }

  if (state != TRACKER_STATE_NONE)
  {
    last_non_none_state = tracker->state;
  }
}

/**
 * Spin in the direction of the last hit to try to find the nest again
 */
void StateMachine::Attacking::search()
{
  int min_ticks = 4;
  int max_ticks = 30;
  int ramp_up_time = 5;
  int ticks_to_skip =  (float)(millis() - enter_millis) / 1000.0 * -(float)(max_ticks - min_ticks) / (float)ramp_up_time + max_ticks;
  ticks_to_skip = cap(ticks_to_skip, min_ticks, max_ticks);

  if (search_timer->check())
  {
    search_count++;
    if (search_count % ticks_to_skip == 0)
    {
      // the right laser hit in the last non none state
      // turn right
      if (last_non_none_state & 1 == 1)
      {
        motor_driver->set(255, -255);
      }
      else
      {
        motor_driver->set(-255, 255);
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
void StateMachine::Attacking::skip(void)
{
  motor_driver->set(255, -255);
  if (millis() - skipping_millis > 2000)
  {
    state = RETURNING_STATE_SEARCHING;
  }
}

void StateMachine::Attacking::measure(void)
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
    if ((float)hit_count / (float(miss_count + hit_count) < 0.8)
    {
      state = RETURNING_STATE_RETURNING;
    }
    // The tape didn't move. Probably not the nest
    else
    {
      skipping_millis = millis();
      // set this so the drone continues in the same direction after it finishes skipping
      last_non_none_state = TRACKER_STATE_LEFT
      state = RETURNING_STATE_SKIPPING;
    }
    miss_count = 0;
    hit_count = 0;
  }
}

void StateMachine::Attacking::drive(void)
{
  switch (tracker->state)
  {
    case TRACKER_STATE_NONE:
      if (last_non_none_state == TRACKER_STATE_LEFT || last_non_none_state == TRACKER_STATE_LEFT_RIGHT)
      {
        motor_driver->set(-255, 255);
      }
      break;
    case TRACKER_STATE_LEFT:
      motor_driver->set(200, 255);
      break;
    case TRACKER_STATE_RIGHT:
      motor_driver->set(255, 200);
      break;
    case TRACKER_STATE_LEFT_RIGHT:
      motor_driver->set(255, 255);
      break;
  }
}