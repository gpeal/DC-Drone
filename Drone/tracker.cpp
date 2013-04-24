#include <stdlib.h>
#include <math.h>
#include "Arduino.h"
#include "Tracker.h"
#include "Utils.h"
#include "StateMachine.h"

// OMGZ SO MUCH HARD WORK!!!

Tracker::Tracker(int _laser_pin, int _transistor_pin_left, int _servo_pin_left, int _transistor_pin_right, int _servo_pin_right)
  : laser_pin(_laser_pin)
{
  left_sensor = new Sensor(_servo_pin_left, _transistor_pin_left, LEFT_EDGE);
  right_sensor = new Sensor(_servo_pin_right, _transistor_pin_right, RIGHT_EDGE);

  pinMode(laser_pin, OUTPUT);
  execute_timer = new Metro(20);
  calibrate(left_sensor);
  calibrate(right_sensor);
}

Tracker::Sensor::Sensor(int _servo_pin, int _transistor_pin, int edge)
  : transistor_pin(_transistor_pin)
{
  servo = new Servo();
  servo->attach(_servo_pin);
  pos = SERVO_MIN_POS;
  servo->write(pos);
  direction = !edge;
  last_reading = 0;
  last_found_pos = (int)((SERVO_MAX_POS - SERVO_MIN_POS) / 2);
  delta_threshold = 40;
  debug->log("Initializing Tracker");
}

void Tracker::calibrate(Sensor *sensor)
{
  int min_deltas[] = {999, 999, 999, 999, 999};
  int delta;

  while(sensor->pos <= SERVO_MAX_POS)
  {
    if (execute_timer->check())
    {
        make_reading(sensor);
        toggle_laser();
        delta = sensor->last_delta;

        if (delta < min_deltas[0] && delta > 1)
          min_deltas[0] = delta;
        else if (delta < min_deltas[1] && delta > 1)
          min_deltas[1] = delta;
        else if (delta < min_deltas[2] && delta > 1)
          min_deltas[2] = delta;
        else if (delta < min_deltas[3] && delta > 1)
          min_deltas[3] = delta;
        else if (delta < min_deltas[4] && delta > 1)
          min_deltas[4] = delta;
        debug->log("Servo Pos: %d", (int)sensor->pos);
        debug->log("Min Deltas: [%d, %d, %d, %d, %d]", min_deltas[0], min_deltas[1], min_deltas[2], min_deltas[3], min_deltas[4]);
        move_servo(sensor, SERVO_RIGHT, 3);
    }
  }
  sensor->delta_threshold = 2 * (min_deltas[0] + min_deltas[1] + min_deltas[2] + min_deltas[3] + min_deltas[4]) / 5.0;
  debug->log("Delta threshold: %d", sensor->delta_threshold);
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
  int adjusted_pos;
  if (direction == SERVO_LEFT)
  {
    sensor->pos -= amount;
  }
  else if (direction == SERVO_RIGHT)
  {
    sensor->pos += amount;
  }
  // debug->log("Pos: %d %d", (int)sensor->pos, (int)amount);

  // in reality, 180 is on the left and 0 is on the right which is reverse of what it
  // logically should be so reverse it here
  adjusted_pos = 180 - sensor->pos;
  sensor->servo->writeMicroseconds(1000.0 + (1000.0/180.0) * adjusted_pos);
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
  make_reading(right_sensor);
  toggle_laser();

  switch(StateMachine::state())
  {
    case StateMachine::SEARCHING:
      if (hit_prey(left_sensor))
      {
        if (left_sensor->pos > SERVO_MIN_POS)
        {
          move_servo(left_sensor, SERVO_LEFT, SERVO_TRACKING_SPEED);
        }
      }
      else if (!hit_prey(left_sensor))
      {
        move_servo(left_sensor, SERVO_RIGHT, SERVO_SEARCHING_SPEED);
        if (left_sensor->pos >= SERVO_MAX_POS)
        {
          move_servo(left_sensor, SERVO_LEFT, left_sensor->pos - SERVO_MIN_POS);
        }
      }
      if (hit_prey(right_sensor))
      {
        if (right_sensor->pos < SERVO_MAX_POS)
        {
          move_servo(right_sensor, SERVO_RIGHT, SERVO_TRACKING_SPEED);
        }
      }
      else if (!hit_prey(right_sensor))
      {
        move_servo(right_sensor, SERVO_LEFT, SERVO_SEARCHING_SPEED);
        if (right_sensor->pos <= SERVO_MIN_POS)
        {
          move_servo(right_sensor, SERVO_RIGHT, SERVO_MAX_POS - right_sensor->pos);
        }
      }
      debug->log("Last Reading: %d\t%d", (int)left_sensor->last_reading, (int)right_sensor->last_reading);
      break;
  }

  // debug->log("Last Reading: %d", (int)(100.0 * (float)left_sensor->last_reading));
  // debug->log("Ratio: %d", (int)(100.0 * ((float)left_sensor->last_delta / (float)delta_threshold)));
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
}

void Tracker::toggle_laser(void)
{
  int laser_value = digitalRead(laser_pin);
  digitalWrite(laser_pin, !laser_value);
}