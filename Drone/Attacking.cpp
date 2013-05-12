#include "Attacking.h"
#include "StateMachine.h"

MotorDriver *StateMachine::Attacking::motor_driver;
Tracker *StateMachine::Attacking::tracker;

void StateMachine::Attacking::loop(void)
{
  tracker->loop();
  switch(tracker->state)
  {
    case TRACKER_STATE_NONE:
      enter(SEARCHING);
      break;
    case TRACKER_STATE_RIGHT:
      motor_driver->set(200, -200);
      break;
    case TRACKER_STATE_MIDDLE:
      motor_driver->set(255, 255);
      break;
    case TRACKER_STATE_MIDDLE_RIGHT:
      motor_driver->set(255, 200);
      break;
    case TRACKER_STATE_LEFT:
      motor_driver->set(-200, 200);
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
}