#include "MotorDriver.h"

MotorDriver::MotorDriver(Motor *left_motor, Motor *right_motor, Odometry *odometry)
  :left_motor(left_motor),
   right_motor(right_motor),
   odometry(odometry)
{
  timer = new Metro(20);
}

void MotorDriver::loop(void)
{
  if (!timer->check())
    return;
  // the angle in which the robot stop and just turns towards the target
  float threshold = 30;
  float _heading_to_target;
  // the amount by which one tread should move faster than the other
  float offset;


  _heading_to_target = heading_to_target();
  // the robot is not facing anywhere near the target. Just turn towards it
  if (abs(_heading_to_target) > threshold)
  {
    if (_heading_to_target < 0)
    {
      left_motor->set(255.0 * SPEED, CW);
      right_motor->set(255.0 * SPEED, CCW);
    }
    else if (_heading_to_target > 0)
    {
      left_motor->set(255.0 * SPEED, CCW);
      right_motor->set(255.0 * SPEED, CW);
    }
  }
  // the robot is facing in the right vicinity
  // move forward with slight alterations
  else
  {
    if (_heading_to_target < 0)
    {
      left_motor->set(255 * SPEED * (0.1 - 0.5 * _heading_to_target / threshold), CW);
      right_motor->set(255 * SPEED, CW);
    }
    else if (_heading_to_target > 0)
    {
      left_motor->set(255 * SPEED, CW);
      right_motor->set(255 * SPEED * (0.1 - 0.5 * _heading_to_target / threshold), CW);
    }
    else
    {
      left_motor->set(255 * SPEED, CW);
      right_motor->set(255 * SPEED, CW);
    }
  }

}

void MotorDriver::move_to(float x, float y)
{
  target.x = x;
  target.y = y;
}

float MotorDriver::heading_to_target(void)
{
  Coordinate diff;
  float heading;
  diff.x = target.x - odometry->position.x;
  diff.y = target.y - odometry->position.y;
  heading = atan2(diff.y, diff.x);
  // first quadrant do nothing
  // second quadrant add 180
  if (diff.y > 0 && diff.x < 0)
    heading += 180;
  // third quadrant add 180
  else if (diff.y < 0 && diff.x < 0)
    heading += 180;
  return heading;
}