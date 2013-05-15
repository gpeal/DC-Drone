#ifndef STATEMACHINE_H
#define STATEMACHINE_H

namespace StateMachine
{
  typedef enum  {
    IDLE = 0,       // 0
    DEPLOYING,      // 1
    SEARCHING,      // 2
    RELOCATING,     // 3
    ATTACKING,      // 4
    SEARCHING_NEST, // 5
    RETURNING,      // 6
    DELIVERING,     // 7
    DISCONNECTED    // 8
  } state_t;
  void enter(state_t state);
  state_t state(void);
  extern state_t _state;
  extern state_t _previous_state;
  extern long enter_millis;
};

#endif