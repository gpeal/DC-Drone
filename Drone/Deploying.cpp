#include <Arduino.h>
#include "Deploying.h"
#include "StateMachine.h"

MotorDriver *StateMachine::Deploying::motor_driver;
bool StateMachine::Deploying::complete;

void StateMachine::Deploying::enter(void)
{
  motor_driver->set(255, 255);
  complete = false;
}

void StateMachine::Deploying::loop(void)
{
  if (millis() - enter_millis > 2000)
  {
    motor_driver->set(0, 0);
    complete = true;
    enter(SEARCHING);
  }
}