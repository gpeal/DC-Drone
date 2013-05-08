#include "Debug.h"
#include "GPS.h"

GPS::GPS(Motor *left_motor, Motor *right_motor, Odometry *odometry)
  :left_motor(left_motor),
   right_motor(right_motor),
   odometry(odometry)
{
  timer = new Metro(20);
}

void GPS::loop(void)
{
  if (targets.isEmpty() || !timer->check())
    return;
  // debug->log("%d,%d->%d,%d", (int)odometry->position.x, (int)odometry->position.y, (int)target.x, (int)target.y);
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
      left_motor->set(255.0 * SPEED, CCW);
      right_motor->set(255.0 * SPEED, CW);
      // debug->log("left");
    }
    else if (_heading_to_target > 0)
    {
      left_motor->set(255.0 * SPEED, CW);
      right_motor->set(255.0 * SPEED, CCW);
      // debug->log("right");
    }
  }
  // the robot is facing in the right vicinity
  // move forward with slight alterations
  else
  {
    offset = 1 - 0.5 * _heading_to_target / threshold;
    if (_heading_to_target < 0)
    {
      left_motor->set(255 * SPEED * offset, CCW);
      right_motor->set(255 * SPEED, CCW);
      // debug->log("sleft %d\t%d", (int)(offset * 100), (int)_heading_to_target);
    }
    else if (_heading_to_target > 0)
    {
      left_motor->set(255 * SPEED, CCW);
      right_motor->set(255 * SPEED * offset, CCW);
      // debug->log("sright %d\t%d", (int)(offset * 100), (int)_heading_to_target);
    }
    else
    {
      left_motor->set(255 * SPEED, CCW);
      right_motor->set(255 * SPEED, CCW);
      // debug->log("straight");
    }
  }
  if (reached_target())
  {
    targets.pop();
  }
}

float GPS::heading_to_target(void)
{
  Coordinate diff;
  float heading;
  diff.x = target.x - odometry->position.x;
  diff.y = target.y - odometry->position.y;
  return odometry->heading - atan2(diff.x, diff.y);
}

double GPS::distance_from_target(void)
{
  return sqrt(pow(target.x - odometry->position.x, 2) + pow(target.y - odometry->position.y, 2));
}

bool GPS::reached_target(void)
{
  return distance_from_target() < 3.0;
}

void GPS::add_waypoint(Coordinate waypoint)
{
  targets.push(waypoint);
}