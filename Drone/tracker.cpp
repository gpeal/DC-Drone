#include "Arduino.h"
#include "Tracker.h"
#include "Utils.h"
#include "StateMachine.h"
#include <stdlib.h>
#include <math.h>

Tracker::Tracker(int _edge_mode, int _edge, int _laser_pin, int _transistor_pin, int _servo_pin)
  : edge_mode(_edge_mode),
    edge(_edge),
    laser_pin(_laser_pin),
    transistor_pin(_transistor_pin)
{
  left_sensor = new Sensor();
  left_sensor->servo = new Servo();
  left_sensor->servo->attach(_servo_pin);
  left_sensor->servo_pos = 0;
  left_sensor->servo->write(left_sensor->servo_pos);
  left_sensor->servo_direction = !edge;
  left_sensor->last_reading = 0;
  left_sensor->last_found_pos = 90;
  delta_threshold = 40;
  debug->log("Attaching servo to port 9");

  pinMode(laser_pin, OUTPUT);
  execute_timer = new Metro(15);
  calibrate();
}

void Tracker::calibrate(void)
{
  int min_deltas[] = {999, 999, 999, 999, 999};
  int delta;

  while(left_sensor->servo_pos < SERVO_MAX_POS)
  {
    if (execute_timer->check())
    {
        make_reading(left_sensor);
        delta = left_sensor->last_delta;
        // debug->log("Delta: %d %d", delta);
        if (delta < min_deltas[0] && delta > 0)
          min_deltas[0] = delta;
        else if (delta < min_deltas[1] && delta > 0)
          min_deltas[1] = delta;
        else if (delta < min_deltas[2] && delta > 0)
          min_deltas[2] = delta;
        else if (delta < min_deltas[3] && delta > 0)
          min_deltas[3] = delta;
        else if (delta < min_deltas[4] && delta > 0)
          min_deltas[4] = delta;
        debug->log("Min Deltas: [%d, %d, %d, %d, %d]", min_deltas[0], min_deltas[1], min_deltas[2], min_deltas[3], min_deltas[4]);
        move_servo(left_sensor, SERVO_RIGHT, 5);
    }
  }
  delta_threshold = 4 * (min_deltas[0] + min_deltas[1] + min_deltas[2] + min_deltas[3] + min_deltas[4]) / 5.0;
  debug->log("Delta threshold: %d", delta_threshold);
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
  return sensor->last_delta > delta_threshold;
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

  if (left_sensor->last_reading != 0)
  {
    debug->log("Ratio: %d", (int)(100.0 * ((float)left_sensor->last_delta / (float)delta_threshold)));
  }
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
  int reading = analogRead(transistor_pin);
  sensor->last_delta = reading - sensor->last_reading;
  sensor->last_reading = reading;
  // laser is off, invert the delta
  if(laser_value == LOW)
  {
    sensor->last_delta *= -1;
  }
  digitalWrite(laser_pin, !laser_value);
}