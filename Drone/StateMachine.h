#ifndef STATEMACHINE_H
#define STATEMACHINE_H

class StateMachine
{
public:
  typedef enum  {
    IDLE = 0,
    DEPLOYING,
    SEARCHING,
    TRACKING,
    ATTACKING,
    CAPTURING,
    RETURNING,
    DELIVERING,
    DISCONNECTED
  } state_t;

  StateMachine(void);
  void enter(state_t state);
  state_t get_state(void);
  static StateMachine *get_instance(void);
private:
  state_t state;
};
static StateMachine *sm_instance;

#endif