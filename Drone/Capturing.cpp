#include "Capturing.h"

MotorDriver *StateMachine::Capturing::motor_driver;
Metro *StateMachine::Capturing::timer = new Metro(300);
int StateMachine::Capturing::state = 1;

void StateMachine::Capturing::loop(void)
{
  if (!timer->check())
    return;

  state *= -1;
  motor_driver->set(state * 255, -state * 255);
}