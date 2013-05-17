#include <Arduino.h>
#include "Debug.h"
#include "Deploying.h"
#include "StateMachine.h"

MotorDriver *StateMachine::Deploying::motor_driver;
bool StateMachine::Deploying::complete;

void StateMachine::Deploying::enter(void)
{
  complete = false;
}

void StateMachine::Deploying::loop(void)
{
  motor_driver->set(255, 255);
  if (millis() - enter_millis > 4000)
  {
    motor_driver->set(0, 0);
    complete = true;
    enter(ATTACKING);
  }
}