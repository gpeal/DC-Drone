#include <stdlib.h>
#include <math.h>
#include "Arduino.h"
#include "Tracker.h"
#include "Utils.h"
#include "StateMachine.h"

// OMGZ SO MUCH HARD WORK!!!

Tracker::Tracker(int transistor_pin_left, int transistor_pin_middle, int transistor_pin_right)
{
  state = TRACKER_STATE_NONE;
  left_sensor = new Sensor(transistor_pin_left);
  middle_sensor = new Sensor(transistor_pin_middle);
  right_sensor = new Sensor(transistor_pin_right);
  state = -1;

  motor_driver = MotorDriver::get_instance();

  execute_timer = new Metro(50);
  left_sensor->calibrate();
  middle_sensor->calibrate();
  right_sensor->calibrate();
}

/**
 * Tracker::loop expects to be called in the arduino main loop and periodically
 *               calls execute to run the control logic on a fixed interval
 *
 */
void Tracker::loop(void)
{
  if (execute_timer->check())
  {
    execute();
  }
}


/**
 * Tracker::execute updates each tracker and runs the logic code for the current state
 *
 */
void Tracker::execute(void)
{
  left_sensor->make_reading();
  middle_sensor->make_reading();
  right_sensor->make_reading();
  Sensor::toggle_laser();

  switch(StateMachine::state())
  {
    case StateMachine::SEARCHING:
    case StateMachine::ATTACKING:
      search();
      // debug->log("Last Reading: %d\t%d", (int)left_sensor->last_reading(), (int)right_sensor->last_reading());
      break;
  }

  // debug->log("LR: %d\t%d", (int)left_sensor->last_reading, (int)right_sensor->last_reading);
  // debug->log("Ratio: %d", (int)(100.0 * ((float)left_sensor->last_delta / (float)delta_threshold)));
}

/**
 * Tracker::search runs the logic for the trackers when it is in the searching state
 *                 Both sensors are used to find the edges of a prey
 *
 */
void Tracker::search(void)
{
  // shorthand for whether or not the left, middle, and right sensors hit
  int left, middle, right, new_state;
  left = left_sensor->recently_hit_prey();
  middle = middle_sensor->recently_hit_prey();
  right = right_sensor->recently_hit_prey();
  new_state = ((int)left_sensor->recently_hit_prey() << 2) | ((int)middle_sensor->recently_hit_prey() << 1) | ((int)right_sensor->recently_hit_prey());
  if (digitalRead(Sensor::laser_pin) == HIGH)
    // debug->log("R: %d\t%d\t%d\t%d", left_sensor->last_delta, middle_sensor->last_delta, right_sensor->last_delta, state);

  if (new_state == state)
    return;

  state = new_state;
  switch(state)
  {
    case TRACKER_STATE_NONE:
      // motor_driver->set(2, -2);
      StateMachine::enter(StateMachine::SEARCHING);
      break;
    case TRACKER_STATE_RIGHT:
      motor_driver->set(175, 120);
      StateMachine::enter(StateMachine::ATTACKING);
      break;
    case TRACKER_STATE_MIDDLE:
      motor_driver->set(140, 150);
      StateMachine::enter(StateMachine::ATTACKING);
      break;
    case TRACKER_STATE_MIDDLE_RIGHT:
      motor_driver->set(170, 150);
      StateMachine::enter(StateMachine::ATTACKING);
      break;
    case TRACKER_STATE_LEFT:
      motor_driver->set(120, 175);
      StateMachine::enter(StateMachine::ATTACKING);
      break;
    case TRACKER_STATE_LEFT_RIGHT:
      motor_driver->set(120, 120);
      StateMachine::enter(StateMachine::ATTACKING);
      break;
    case TRACKER_STATE_LEFT_MIDDLE:
      motor_driver->set(150, 160);
      StateMachine::enter(StateMachine::ATTACKING);
      break;
    case TRACKER_STATE_LEFT_MIDDLE_RIGHT:
      motor_driver->set(120, 120);
      StateMachine::enter(StateMachine::ATTACKING);
      break;
  }
}