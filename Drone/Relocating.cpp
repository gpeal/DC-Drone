#include "Debug.h"
#include "Relocating.h"
#include "Sonar.h"
#include "StateMachine.h"

MotorDriver *StateMachine::Relocating::motor_driver;
long StateMachine::Relocating::relocating_start_time;
long StateMachine::Relocating::spinning_start_time;

void StateMachine::Relocating::enter(void)
{
  motor_driver->set(255, -255);
  relocating_start_time = 0;
  spinning_start_time = millis();
}

void StateMachine::Relocating::loop(void)
{
  Sonar::loop();
  if (!relocating() && (Sonar::prey_inches > THRESHOLD_DISTANCE || millis() - spinning_start_time > MAX_RELOCATION_TIME))
  {
    relocating_start_time = millis();
    motor_driver->set(255, 255);
  }
  if (relocating() && (Sonar::prey_inches < RELOCATED_DISTANCE || millis() - relocating_start_time > MAX_RELOCATION_TIME))
  {
    motor_driver->set(0, 0);
    StateMachine::enter(StateMachine::_previous_state);
  }
}