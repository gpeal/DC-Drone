#include <stdlib.h>
#include <math.h>
#include "Arduino.h"
#include "Tracker.h"
#include "Utils.h"
#include "StateMachine.h"

/*
 * PHOTOTRANSISTOR CIRCUIT
 * RED END -> ANALOG IN AND 47K -> GROUND
 * BLACK END -> +5V
 * The colors are reversed because the red end is actually just the emitter and the black end is just the collector
 */

// OMGZ SO MUCH HARD WORK!!!

Tracker::Tracker(int transistor_pin_left, int transistor_pin_middle, int transistor_pin_right)
{
  state = TRACKER_STATE_NONE;
  left_sensor = new Sensor(transistor_pin_left);
  middle_sensor = new Sensor(transistor_pin_middle);
  right_sensor = new Sensor(transistor_pin_right);
  state = -1;

  motor_driver = MotorDriver::get_instance();

  execute_timer = new Metro(20);
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

  // debug->log("LR: %d\t%d\t%d", (int)left_sensor->last_delta, (int)middle_sensor->last_delta, (int)right_sensor->last_delta);
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
  // if (digitalRead(Sensor::laser_pin) == HIGH)
    // debug->log("S%d", state);
  if (new_state == state)
    return;

  state = new_state;
  // remove wheel momentum which would cause it to turn initially when
  // going from spinning to going straight
  switch(state)
  {
    case TRACKER_STATE_NONE:
      StateMachine::enter(StateMachine::SEARCHING);
      break;
    case TRACKER_STATE_RIGHT:
      motor_driver->set(225, 0.4);
      StateMachine::enter(StateMachine::ATTACKING);
      break;
    case TRACKER_STATE_MIDDLE:
      motor_driver->set(225, 1.3);
      StateMachine::enter(StateMachine::ATTACKING);
      break;
    case TRACKER_STATE_MIDDLE_RIGHT:
      motor_driver->set(225, 0.5);
      StateMachine::enter(StateMachine::ATTACKING);
      break;
    case TRACKER_STATE_LEFT:
      motor_driver->set(140, 1.7);
      StateMachine::enter(StateMachine::ATTACKING);
      break;
    case TRACKER_STATE_LEFT_RIGHT:
      motor_driver->set(150, 1.0);
      StateMachine::enter(StateMachine::ATTACKING);
      break;
    case TRACKER_STATE_LEFT_MIDDLE:
      motor_driver->set(150, 1.5);
      StateMachine::enter(StateMachine::ATTACKING);
      break;
    case TRACKER_STATE_LEFT_MIDDLE_RIGHT:
      motor_driver->set(225, 1.0);
      StateMachine::enter(StateMachine::ATTACKING);
      break;
  }
}