#include "Arduino.h"
#include "StateMachine.h"

StateMachine::StateMachine(void)
{
  state = IDLE;
}

void StateMachine::enter(state_t state)
{
  this->state = state;
}

StateMachine::state_t StateMachine::get_state(void)
{
  return state;
}

StateMachine *StateMachine::get_instance(void)
{
  if (sm_instance == NULL)
  {
    sm_instance = new StateMachine();
  }
  return sm_instance;
}