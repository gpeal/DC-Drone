#include "Arduino.h"
#include "Tracker.h"
#include <stdlib.h>

Tracker::Tracker(int _edge_mode, int _edge, int _laser_pin, int _transistor_pin)
  : edge_mode(_edge_mode),
    edge(_edge),
    laser_pin(_laser_pin),
    transistor_pin(_transistor_pin)
{
  left_sensor = new Sensor();
  left_sensor->servo = new Servo();
  left_sensor->servo->attach(9);
  left_sensor->servo_pos = 90;
  left_sensor->servo->write(left_sensor->servo_pos);
  left_sensor->servo_direction = !edge;
  left_sensor->last_reading = 0;
  reading_threshold = 40;
  debug->log("Attaching servo to port 9");

  laser_timer = new Timer();
  pinMode(laser_pin, OUTPUT);
  laser_timer->every(50, this, execute_wrapper);
}

void Tracker::loop(void)
{
  laser_timer->update();
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
    debug->log("Moving servo left");
  }
  else if (direction == SERVO_RIGHT && sensor->servo_pos < SERVO_MAX_POS)
  {
    sensor->servo_pos += amount;
    debug->log("Moving servo right");
  }

  sensor->servo->writeMicroseconds(1000 + (1000.0/180.0) * sensor->servo_pos);
}

void Tracker::execute(void)
{
  int delta;
  if (edge_mode == RIGHT_EDGE)
  {
    delta = make_reading(right_sensor);
  }
  else if (edge_mode == LEFT_EDGE)
  {
    delta = make_reading(left_sensor);
  }

  if (delta < reading_threshold)
  {
    move_servo(left_sensor, left_sensor->servo_direction);
    // TODO: add right sensor
    if (left_sensor->servo_pos <= SERVO_MIN_POS || left_sensor->servo_pos >= SERVO_MAX_POS)
    {
      reverse_servo(left_sensor);
    }
  }
  else // hit target
  {
    left_sensor->last_found_position = left_sensor->servo_pos;
    if (left_sensor, edge_mode == LEFT_EDGE)
    {
      move_servo(left_sensor, SERVO_LEFT);
      left_sensor->servo_direction = SERVO_RIGHT;
    }
    // TODO: add both edges
  }
  debug->log("Delta: %d\tServo Pos: %d", delta, left_sensor->servo_pos);
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

int Tracker::make_reading(Sensor *sensor)
{
  int laser_value = digitalRead(laser_pin);
  int reading = analogRead(transistor_pin);
  int delta = reading - sensor->last_reading;
  sensor->last_reading = reading;
  // laser is off, invert the delta
  if(laser_value == LOW)
  {
    delta *= -1;
  }
  digitalWrite(laser_pin, !laser_value);
  return delta;
}

void Tracker::execute_wrapper(void *self)
{
  ((Tracker *)self)->execute();
}