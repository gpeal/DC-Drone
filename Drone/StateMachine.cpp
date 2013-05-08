#include "Arduino.h"
#include "StateMachine.h"

StateMachine::state_t StateMachine::_state = SEARCHING;
long StateMachine::enter_millis = millis();
/**
 * StateMachine::enter takes a state_t and set the state to that
 * right now all it does is set the state and nothing else.
 * In the future, there can be logic associated with it.
 *
 * @param state the state to enter
 */
void StateMachine::enter(state_t new_state)
{
  _state = new_state;
  enter_millis = millis();
}

StateMachine::state_t StateMachine::state(void)
{
  return _state;
}