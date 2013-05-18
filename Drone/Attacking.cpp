#include "Attacking.h"
#include "StateMachine.h"
#include "Sonar.h"

MotorDriver *StateMachine::Attacking::motor_driver;
Tracker *StateMachine::Attacking::tracker;
int StateMachine::Attacking::last_non_none_state;
long StateMachine::Attacking::last_hit_millis;
int StateMachine::Attacking::captured;
Servo *StateMachine::Attacking::left_servo;
Servo *StateMachine::Attacking::right_servo;

void StateMachine::Attacking::enter(void)
{
  last_non_none_state = TRACKER_STATE_NONE;
  last_hit_millis = millis();
  captured = 0;
  left_servo->write(0);
  right_servo->write(100);
}

void StateMachine::Attacking::loop(void)
{
  int duty, direction, tracker_state;

  tracker->loop();

  if (tracker->state == TRACKER_STATE_NONE)
  {
    if (millis() - last_hit_millis > 12000)
    {
      enter(RELOCATING);
      return;
    }
  }
  else // state non none
  {
    last_non_none_state = tracker->state;
    last_hit_millis = millis();
    Sonar::loop();
  }
  switch(tracker->state)
  {
    case TRACKER_STATE_NONE:
      if (last_non_none_state >> 2 & 1 == 1)
        motor_driver->spin(7, 33, 3, LEFT);
      else
        motor_driver->spin(7, 33, 3, RIGHT);
      break;
    case TRACKER_STATE_RIGHT:
      motor_driver->spin(15, RIGHT);
      break;
    case TRACKER_STATE_MIDDLE:
      motor_driver->set(255, 255);
      break;
    case TRACKER_STATE_MIDDLE_RIGHT:
      motor_driver->set(255, 220);
      break;
    case TRACKER_STATE_LEFT:
      motor_driver->spin(15, LEFT);
      break;
    case TRACKER_STATE_LEFT_RIGHT:
      motor_driver->set(255, 255);
      break;
    case TRACKER_STATE_LEFT_MIDDLE:
      motor_driver->set(220, 255);
      break;
    case TRACKER_STATE_LEFT_MIDDLE_RIGHT:
      motor_driver->set(255, 255);
      break;
  }
  // check if we captured the prey
  // if middle laser hit and distance < 1.6 in
  // an undefined reading returns 0 so ignore that too
  if (tracker->state >> 1 & 1 == 1 && Sonar::prey_inches < 1.6)
  {
    captured = 1;
    left_servo->write(100);
    right_servo->write(0);
    enter(RETURNING);
    delay(3000);
  }
}