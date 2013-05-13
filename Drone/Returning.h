#ifndef RETURNING_H
#define RETURNING_H
/*
 * The StateMachine::Returning namespace should handle all logic specific to the RETURNING state
 * There is a loop function that is expected to be run every iteration of the main loop as long
 * as the state is RETURNING
 */

#include "Metro.h"
#include "tracker.h"

// no retroreflective tape found
#define RETURNING_STATE_SEARCHING 0
// determining whether the retroreflective tape is from our nest
#define RETURNING_STATE_MEASURING 1
// found retroreflective tap that isn't the nest. skip it
#define RETURNING_STATE_SKIPPING 2
// found the nest, returning
#define RETURNING_STATE_RETURNING 3

namespace StateMachine
{
  namespace Returning
  {
    void enter(void);
    void loop(void);
    void search(void);
    void measure(void);
    void skip(void);
    void drive(void);
    extern Tracker *tracker;
    extern MotorDriver *motor_driver;
    extern int hit_count;
    extern int miss_count;
    extern int state;
    extern int last_non_none_state;
    extern Metro *search_timer;
    extern int search_count;
    // the time the drone started rotating to avoid the retroreflective tape that it
    // found that isn't the nest
    extern long skipping_millis;
    extern long start_search_millis;
  }
}

#endif