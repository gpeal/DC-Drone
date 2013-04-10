#include "Arduino.h"
#include "StateMachine.h"

StateMachine::state_t StateMachine::_state = IDLE;
/**
 * StateMachine::enter takes a state_t and set the state to that
 * right now all it does is set the state and nothing else.
 * In the future, there can be logic associated with it.
 *
 * @param state the state to enter
 */
void StateMachine::enter(state_t state_)
{
  // state = state_;
}

StateMachine::state_t StateMachine::state(void)
{
  return _state;
}