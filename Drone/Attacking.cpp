#include "Attacking.h"
#include "StateMachine.h"
#include "Sonar.h"

MotorDriver *StateMachine::Attacking::motor_driver;
Tracker *StateMachine::Attacking::tracker;
int StateMachine::Attacking::last_non_none_state;
long StateMachine::Attacking::last_hit_millis;
int StateMachine::Attacking::captured;
const int StateMachine::Attacking::LEFT = -1;
const int StateMachine::Attacking::RIGHT = 1;

void StateMachine::Attacking::enter(void)
{
  last_non_none_state = TRACKER_STATE_NONE;
  last_hit_millis = 0;
  captured = 0;
}

void StateMachine::Attacking::loop(void)
{
  int duty, direction, tracker_state;

  if (tracker->loop())
  {
    Sonar::loop();
    debug->log("T%d", tracker->state);
  }

  if (tracker->state != TRACKER_STATE_NONE)
  {
    last_non_none_state = tracker->state;
    last_hit_millis = millis();
  }

  // prevent the drone from running into a wall
  // if (tracker->state >> 1 & 1 != 1 && Sonar::prey_inches < 12 && Sonar::prey_inches > 0.1)
  // {
  //   tracker_state = TRACKER_STATE_NONE;
  // }
  // else
  // {
  //   tracker_state = tracker->state;
  // }

  switch(tracker->state)
  {
    case TRACKER_STATE_NONE:
      // the right laser hit in the last non none state
      if (last_non_none_state & 1 == 1)
      {
        direction = RIGHT;
      }
      else
      {
        direction = LEFT;
      }
      if (millis() - last_hit_millis < 2000)
      {
        duty = 10;
      }
      else
      {
        duty = 30;
      }
      spin(duty, direction);
      break;
    case TRACKER_STATE_RIGHT:
      motor_driver->set(180, -180);
      break;
    case TRACKER_STATE_MIDDLE:
      motor_driver->set(255, 255);
      break;
    case TRACKER_STATE_MIDDLE_RIGHT:
      motor_driver->set(255, 200);
      break;
    case TRACKER_STATE_LEFT:
      motor_driver->set(-180, 180);
      break;
    case TRACKER_STATE_LEFT_RIGHT:
      motor_driver->set(255, 255);
      break;
    case TRACKER_STATE_LEFT_MIDDLE:
      motor_driver->set(200, 255);
      break;
    case TRACKER_STATE_LEFT_MIDDLE_RIGHT:
      motor_driver->set(255, 255);
      break;
  }
  // check if we captured the prey
  // if middle laser hit and distance < 1.6 in
  // an undefined reading returns 0 so ignore that too
  if (tracker->state >> 1 & 1 == 1 && Sonar::prey_inches < 1.6 && Sonar::prey_inches > 0.1)
  {
    captured = 1;
    enter(RETURNING);
  }
}

/**
 * Turn the motors on for duty millis every 100 millis
 * Direction takes RIGHT or LEFT
 */
void StateMachine::Attacking::spin(int duty, int direction)
{
  if (millis() % 100 < duty)
  {
    motor_driver->set(direction * 255,  -direction * 255);
  }
  else
  {
    motor_driver->set(0, 0);
  }
}