#include "Arduino.h"
#include "Tracker.h"
#include "Utils.h"
#include <stdlib.h>
#include <math.h>

Tracker::Tracker(int _edge_mode, int _edge, int _laser_pin, int _transistor_pin)
  : edge_mode(_edge_mode),
    edge(_edge),
    laser_pin(_laser_pin),
    transistor_pin(_transistor_pin)
{
  left_sensor = new Sensor();
  left_sensor->servo = new Servo();
  left_sensor->servo->attach(9);
  left_sensor->servo_pos = 0;
  left_sensor->servo->write(left_sensor->servo_pos);
  left_sensor->servo_direction = !edge;
  left_sensor->last_reading = 0;
  left_sensor->last_found_pos = 90;
  reading_threshold = 40;
  debug->log("Attaching servo to port 9");

  pinMode(laser_pin, OUTPUT);
  execute_timer = new Metro(50);
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
        delta = make_reading(left_sensor);
        debug->log("Delta: %d", delta);
        if (delta < min_deltas[0] && delta != 0)
          min_deltas[0] = delta;
        else if (delta < min_deltas[1] && delta != 0)
          min_deltas[1] = delta;
        else if (delta < min_deltas[2] && delta != 0)
          min_deltas[2] = delta;
        else if (delta < min_deltas[3] && delta != 0)
          min_deltas[3] = delta;
        else if (delta < min_deltas[4] && delta != 0)
          min_deltas[4] = delta;
        debug->log("Min Deltas: [%d, %d, %d, %d, %d]", min_deltas[0], min_deltas[1], min_deltas[2], min_deltas[3], min_deltas[4]);
        move_servo(left_sensor, SERVO_RIGHT, 5);
    }
  }
  reading_threshold = 4 * (min_deltas[0] + min_deltas[1] + min_deltas[2] + min_deltas[3] + min_deltas[4]) / 5.0;
  debug->log("Reading threshold: %d", reading_threshold);
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

  sensor->servo->writeMicroseconds(1000 + (1000.0/180.0) * sensor->servo_pos);
}

void Tracker::execute(void)
{
  int delta;
  float servo_speed;
  delta = make_reading(left_sensor);

  if (delta < reading_threshold)
  {
    servo_speed = abs(left_sensor->servo_pos - left_sensor->last_found_pos) / 90.0;
    servo_speed = cap(servo_speed, SERVO_MIN_SPEED, SERVO_MAX_SPEED);
    // debug->log("Servo Speed: %f", servo_speed);
    move_servo(left_sensor, left_sensor->servo_direction, servo_speed);
    // TODO: add right sensor
    if (left_sensor->servo_pos <= SERVO_MIN_POS || left_sensor->servo_pos >= SERVO_MAX_POS)
    {
      reverse_servo(left_sensor);
    }
  }
  else // hit target
  {
    left_sensor->last_found_pos = left_sensor->servo_pos;
    servo_speed = SERVO_MAX_SPEED;
    move_servo(left_sensor, SERVO_LEFT, servo_speed);
    left_sensor->servo_direction = SERVO_RIGHT;
    // TODO: add both edges
  }
  debug->log("Delta: %d\tServo Pos: %d\tServo Speed: %d", delta, (int)(100*left_sensor->servo_pos), (int)(100*servo_speed));
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