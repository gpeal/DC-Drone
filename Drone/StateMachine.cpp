#include "Arduino.h"
#include "Attacking.h"
#include "Deploying.h"
#include "Relocating.h"
#include "Returning.h"
#include "StateMachine.h"

StateMachine::state_t StateMachine::_state;
StateMachine::state_t StateMachine::_previous_state;
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
  _previous_state = _state;
  //call leave functions
  switch(_previous_state)
  {
  }

  _state = new_state;
  enter_millis = millis();
  switch(_state)
  {
    case DEPLOYING:
      Deploying::enter();
      debug->log("Deploying");
      break;
    case ATTACKING:
      Attacking::enter();
      debug->log("Attacking");
      break;
    case RELOCATING:
      Relocating::enter();
      debug->log("Relocating");
      break;
    case RETURNING:
      Returning::enter();
      debug->log("Returning");
      break;
  }
}

StateMachine::state_t StateMachine::state(void)
{
  return _state;
}