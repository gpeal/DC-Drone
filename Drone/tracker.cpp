#include <stdlib.h>
#include <math.h>
#include "Arduino.h"
#include "Tracker.h"
#include "Utils.h"
#include "StateMachine.h"

Tracker::Tracker(int _laser_pin, int _transistor_pin_left, int _servo_pin_left, int _transistor_pin_right, int _servo_pin_right)
  : laser_pin(_laser_pin)
{
  left_sensor = new Sensor(_servo_pin_left, _transistor_pin_right, LEFT_EDGE);
  right_sensor = new Sensor(_servo_pin_right, _transistor_pin_left, RIGHT_EDGE);

  pinMode(laser_pin, OUTPUT);
  execute_timer = new Metro(50);
  calibrate();
}

Tracker::Sensor::Sensor(int _servo_pin, int _transistor_pin, int edge)
  : transistor_pin(_transistor_pin)
{
  servo = new Servo();
  servo->attach(_servo_pin);
  servo_pos = SERVO_MIN_POS;
  servo->write(servo_pos);
  servo_direction = !edge;
  last_reading = 0;
  last_found_pos = (int)((SERVO_MAX_POS - SERVO_MIN_POS) / 2);
  delta_threshold = 40;
  debug->log("Initializing Tracker");
}

void Tracker::calibrate(void)
{
  int min_deltas_right[] = {999, 999, 999, 999, 999};
  int min_deltas_left[] = {999, 999, 999, 999, 999};
  int delta_left;
  int delta_right;

  while(left_sensor->servo_pos < SERVO_MAX_POS)
  {
    if (execute_timer->check())
    {
        make_reading(left_sensor);
        make_reading(right_sensor);
        delta_left = left_sensor->last_delta;
        delta_right = right_sensor->last_delta;

        if (delta_left < min_deltas_left[0] && delta_left > 0)
          min_deltas_left[0] = delta_left;
        else if (delta_left < min_deltas_left[1] && delta_left > 0)
          min_deltas_left[1] = delta_left;
        else if (delta_left < min_deltas_left[2] && delta_left > 0)
          min_deltas_left[2] = delta_left;
        else if (delta_left < min_deltas_left[3] && delta_left > 0)
          min_deltas_left[3] = delta_left;
        else if (delta_left < min_deltas_left[4] && delta_left > 0)
          min_deltas_left[4] = delta_left;

        if (delta_right < min_deltas_right[0] && delta_right > 0)
          min_deltas_right[0] = delta_right;
        else if (delta_right < min_deltas_right[1] && delta_right > 0)
          min_deltas_right[1] = delta_right;
        else if (delta_right < min_deltas_right[2] && delta_right > 0)
          min_deltas_right[2] = delta_right;
        else if (delta_right < min_deltas_right[3] && delta_right > 0)
          min_deltas_right[3] = delta_right;
        else if (delta_right < min_deltas_right[4] && delta_right > 0)
          min_deltas_right[4] = delta_right;
        debug->log("Min Deltas: [%d, %d, %d, %d, %d] [%d, %d, %d %d, %d]", min_deltas_left[0], min_deltas_left[1], min_deltas_left[2], min_deltas_left[3], min_deltas_left[4], min_deltas_right[0], min_deltas_right[1], min_deltas_right[2], min_deltas_right[3], min_deltas_right[4]);
        move_servo(left_sensor, SERVO_RIGHT, 3);
        move_servo(right_sensor, SERVO_RIGHT, 3);
    }
  }
  left_sensor->delta_threshold = 4 * (min_deltas_left[0] + min_deltas_left[1] + min_deltas_left[2] + min_deltas_left[3] + min_deltas_left[4]) / 5.0;
  right_sensor->delta_threshold = 4 * (min_deltas_right[0] + min_deltas_right[1] + min_deltas_right[2] + min_deltas_right[3] + min_deltas_right[4]) / 5.0;
  debug->log("Delta threshold: %d\t%d", left_sensor->delta_threshold, right_sensor->delta_threshold);
}

void Tracker::loop(void)
{
  if (execute_timer->check())
  {
    execute();
  }
}

void Tracker::move_servo(Sensor *sensor, int direction)
{
  move_servo(sensor, direction, 1);
}


void Tracker::move_servo(Sensor *sensor, int direction, float amount)
{
  if (direction == SERVO_LEFT && sensor->servo_pos > SERVO_MIN_POS)
  {
    sensor->servo_pos -= amount;
  }
  else if (direction == SERVO_RIGHT && sensor->servo_pos < SERVO_MAX_POS)
  {
    sensor->servo_pos += amount;
  }

  sensor->servo->writeMicroseconds(1000.0 + (1000.0/180.0) * sensor->servo_pos);
}

/**
 * Tracker::hit_prey takes a sensor and returns whether the sensor hit a prey on its last reading
 *
 * @param  sensor the sensor to check
 *
 * @return 0 or 1 whether or not it hit a prey
 */
int Tracker::hit_prey(Sensor *sensor)
{
  return sensor->last_delta > sensor->delta_threshold;
}

void Tracker::execute(void)
{
  make_reading(left_sensor);

  switch(StateMachine::state())
  {
    case StateMachine::SEARCHING:
      if (hit_prey(left_sensor))
      {
        move_servo(left_sensor, SERVO_RIGHT, SERVO_SEARCHING_SPEED);
        // prevent the servo from getting stuck on the right side
        if (left_sensor->servo_pos > SERVO_MAX_POS)
        {
          move_servo(left_sensor, SERVO_LEFT, 45);
        }
      }
      else if (!hit_prey(left_sensor))
      {
        move_servo(left_sensor, SERVO_LEFT, SERVO_SEARCHING_SPEED);
        if (left_sensor->servo_pos < SERVO_MIN_POS)
        {
          move_servo(left_sensor, SERVO_RIGHT, 45);
        }
      }
      break;
  }

  debug->log("Last Reading: %d", (int)(100.0 * (float)left_sensor->last_reading));
  // debug->log("Ratio: %d", (int)(100.0 * ((float)left_sensor->last_delta / (float)delta_threshold)));
}

void Tracker::reverse_servo(Sensor *sensor)
{
  if (sensor->servo_direction == SERVO_RIGHT)
  {
    sensor->servo_direction = SERVO_LEFT;
  }
  else if (sensor->servo_direction == SERVO_LEFT)
  {
    sensor->servo_direction = SERVO_RIGHT;
  }
}

void Tracker::make_reading(Sensor *sensor)
{
  int laser_value = digitalRead(laser_pin);
  int reading = analogRead(sensor->transistor_pin);
  sensor->last_delta = reading - sensor->last_reading;
  sensor->last_reading = reading;
  // laser is off, invert the delta
  if(laser_value == LOW)
  {
    sensor->last_delta *= -1;
  }
  digitalWrite(laser_pin, !laser_value);
}