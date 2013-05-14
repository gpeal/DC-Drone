#ifndef STATEMACHINE_H
#define STATEMACHINE_H

namespace StateMachine
{
  typedef enum  {
    IDLE = 0,
    DEPLOYING,
    SEARCHING,
    RELOCATING,
    ATTACKING,
    SEARCHING_NEST,
    RETURNING,
    DELIVERING,
    DISCONNECTED
  } state_t;
  void enter(state_t state);
  state_t state(void);
  extern state_t _state;
  extern state_t _previous_state;
  extern long enter_millis;
};

#endif