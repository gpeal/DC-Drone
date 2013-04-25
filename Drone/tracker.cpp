#include <stdlib.h>
#include <math.h>
#include "Arduino.h"
#include "Tracker.h"
#include "Utils.h"
#include "StateMachine.h"

// OMGZ SO MUCH HARD WORK!!!

Tracker::Tracker(int _transistor_pin_left, int _servo_pin_left, int _transistor_pin_right, int _servo_pin_right)
{
  state = TRACKER_STATE_NEITHER;
  left_sensor = new Sensor(_servo_pin_left, _transistor_pin_left, LEFT_EDGE);
  right_sensor = new Sensor(_servo_pin_right, _transistor_pin_right, RIGHT_EDGE);
  prey_position_buffer_i = 0;

  execute_timer = new Metro(20);
  left_sensor->calibrate();
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
  if (state == TRACKER_STATE_BOTH)
  {
    debug->log("Prey Pos: %d", (int)prey_position);
    // int left_ratio = (int)(100.0 * ((float)left_sensor->last_delta / (float)left_sensor->delta_threshold));
    // int right_ratio = (int)(100.0 * ((float)right_sensor->last_delta / (float)right_sensor->delta_threshold));
    // if (left_ratio >= 100 || right_ratio >= 100)
    //   debug->log("Ratio   : %d\t%d", left_ratio, right_ratio);
  }
}


/**
 * Tracker::execute updates each tracker and runs the logic code for the current state
 *
 */
void Tracker::execute(void)
{
  left_sensor->make_reading();
  right_sensor->make_reading();
  Sensor::toggle_laser();

  switch(StateMachine::state())
  {
    case StateMachine::SEARCHING:
      search();
      //debug->log("Last Reading: %d\t%d", (int)left_sensor->last_reading, (int)right_sensor->last_reading);
      break;
  }

  // debug->log("Last Reading: %d", (int)(left_sensor->last_reading));
  // debug->log("Ratio: %d", (int)(100.0 * ((float)left_sensor->last_delta / (float)delta_threshold)));
}

/**
 * Tracker::search runs the logic for the trackers when it is in the searching state
 *                 Both sensors are used to find the edges of a prey
 *
 */
void Tracker::search(void)
{
  //left sensor
  if (left_sensor->hit_prey())
  {
    left_sensor->last_found_millis = millis();
    left_sensor->last_found_pos = left_sensor->pos;
    if (left_sensor->pos > SERVO_MIN_POS)
    {
      left_sensor->move_servo(SERVO_LEFT, SERVO_TRACKING_SPEED);
    }
  }
  else if (!left_sensor->hit_prey())
  {
    left_sensor->move_servo(SERVO_RIGHT, SERVO_SEARCHING_SPEED);
    if (left_sensor->pos >= SERVO_MAX_POS)
    {
      left_sensor->move_servo(SERVO_LEFT, left_sensor->pos - SERVO_MIN_POS);
    }
  }
  // right sensor
  if (right_sensor->hit_prey())
  {
    right_sensor->last_found_millis = millis();
    right_sensor->last_found_pos = right_sensor->pos;
    if (right_sensor->pos < SERVO_MAX_POS)
    {
      right_sensor->move_servo(SERVO_RIGHT, SERVO_TRACKING_SPEED);
    }
  }
  else if (!right_sensor->hit_prey())
  {
    right_sensor->move_servo(SERVO_LEFT, SERVO_SEARCHING_SPEED);
    if (right_sensor->pos <= SERVO_MIN_POS)
    {
      right_sensor->move_servo(SERVO_RIGHT, SERVO_MAX_POS - right_sensor->pos);
    }
  }
  // update tracker state
  state = ((int)left_sensor->recently_hit_prey() << 1) | ((int)right_sensor->recently_hit_prey());
  update_prey_position();
}

/**
 * Tracker::prey_position
 *
 * @return If both trackers are tracking, it returns the best approximation
 *         of the middle of the prey using a MAF5
 *         If either tracker isn't tracking, returns -1
 */
void Tracker::update_prey_position(void)
{
  float sum = 0;
  if (state == TRACKER_STATE_BOTH)
  {
    prey_position_buffer[(prey_position_buffer_i++)%PREY_POSITION_BUFFER_SIZE] =
      (right_sensor->pos + left_sensor->pos) / 2.0;
    for (int i = 0; i < PREY_POSITION_BUFFER_SIZE; i++)
    {
      sum += prey_position_buffer[i];
    }
    prey_position = sum / PREY_POSITION_BUFFER_SIZE;
  }
}