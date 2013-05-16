#include "Attacking.h"
#include "Motor.h"
#include "Returning.h"
#include "Sonar.h"
#include "StateMachine.h"
#include "Utils.h"

Tracker *StateMachine::Returning::tracker;
MotorDriver *StateMachine::Returning::motor_driver;
int StateMachine::Returning::returning_state;
long StateMachine::Returning::returning_state_enter_millis;
int StateMachine::Returning::hit_count;
int StateMachine::Returning::miss_count;
int StateMachine::Returning::last_non_none_state;
long StateMachine::Returning::last_non_none_millis;
const int StateMachine::Returning::LEFT = -1;
const int StateMachine::Returning::RIGHT = 1;

/**
 * This is automatically called when the state is entered
 */
void StateMachine::Returning::enter(void)
{
  enter(RETURNING_STATE_SEARCHING);
  last_non_none_state = TRACKER_STATE_NONE;
  last_non_none_millis = millis();
}

void StateMachine::Returning::loop(void)
{
  if (tracker->loop())
  {
    Sonar::loop();
    debug->log("S:%d", returning_state);
  }

  if (tracker->state != TRACKER_STATE_NONE)
  {
    last_non_none_state = tracker->state;
    last_non_none_millis = millis();
  }

  switch (returning_state)
  {
    case RETURNING_STATE_SEARCHING:
      search();
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

  if (tracker->state != TRACKER_STATE_NONE && returning_state != RETURNING_STATE_SKIPPING)
  {
    last_non_none_state = tracker->state;
  }
}

/**
 * Spin in the direction of the last hit to try to find the nest again
 */
void StateMachine::Returning::search()
{
  if (tracker->state == TRACKER_STATE_NONE)
  {
    if (last_non_none_state & 1 == 1)
    {
      spin(13, RIGHT);
    }
    else
    {
      spin(13, LEFT);
    }
  }
  else
  {
    enter(RETURNING_STATE_MEASURING);
  }
}

/*
 * spin to avoid the retroreflective tape that isn't the nest
 */
void StateMachine::Returning::skip(void)
{
  if (millis() - returning_state_enter_millis > 1000)
  {
    enter(RETURNING_STATE_SEARCHING);
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
  if (millis() - returning_state_enter_millis > 2000)
  {
    // less than 80% of the time it was hitting the tape
    // that means it was probably moving
    if ((float)hit_count / (float)(miss_count + hit_count) < 0.8)
    {
      enter(RETURNING_STATE_RETURNING);
    }
    // The tape didn't move. Probably not the nest
    else
    {
      enter(RETURNING_STATE_SKIPPING);
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
      if (last_non_none_state & 1 == 1)
      {
        spin(8, RIGHT);
      }
      else
      {
        spin(8, LEFT);
      }
      break;
    case TRACKER_STATE_LEFT:
      motor_driver->set(245, 255);
      break;
    case TRACKER_STATE_RIGHT:
      motor_driver->set(255, 245);
      break;
    case TRACKER_STATE_LEFT_RIGHT:
      motor_driver->set(255, 255);
      break;
  }
}

/**
 * Turn the motors on for duty millis every 100 millis
 * Direction takes RIGHT or LEFT
 */
void StateMachine::Returning::spin(int duty, int direction)
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

void StateMachine::Returning::enter(int state)
{
  returning_state = state;
  returning_state_enter_millis = millis();

  switch(returning_state)
  {
    case RETURNING_STATE_MEASURING:
      hit_count = 0;
      miss_count = 0;
      motor_driver->set(0, 0);
  }
}