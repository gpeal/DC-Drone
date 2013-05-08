#include "Debug.h"
#include "MotorDriver.h"

MotorDriver::MotorDriver()
{
  timer = new Metro(100);
}

void MotorDriver::loop(void)
{
  // TODO remove this
  return;
  int left_speed, right_speed;
  float current_bias;
  if (target_bias == 0 || !timer->check())
    return;

  current_bias = (float)(odometry->left_reading - starting_left_reading) /
         (float)(odometry->right_reading - starting_right_reading);


  if (current_bias > target_bias)
    bias *= 0.9;
  else if (current_bias < target_bias)
    bias *= 1.1;

  bias = cap(bias, 0.4, 1.6);

  left_speed = cap((int)((float)target_speed * bias), 0, 255);
  right_speed = target_speed;
  // debug->log("D:%d\t%d\t%d\t%d\t%d\t%d\t%d", (int)(odometry->left_reading - starting_left_reading),
  //                        (int)(odometry->right_reading - starting_right_reading),
  //                        (int)(current_bias * 100.0),
  //                        (int)(bias * 100),
  //                        (int)(target_bias * 100.0),
  //                        left_speed,
  //                        right_speed);
  left_motor->set(left_speed, left_direction);
  right_motor->set(right_speed, right_direction);
}

/**
 * MotorDriver::set sets the desired velocity of each tread
 * int speed sets the desired speed
 * float target_bias sets the ration of left encoder counts / right encoder counts
 * a target_bias of 0 will make the treads spin in opposite directions
 *
 * negative values turns the tread backwards
 * a good velocity is on the order of 300
 */
void MotorDriver::set(int target_speed, float target_bias)
{
  // TODO remove this
  return;
  int left_speed1, left_speed, right_speed;
  this->target_bias = target_bias;
  this->target_speed = target_speed;
  starting_left_reading = odometry->left_reading;
  starting_right_reading = odometry->right_reading;

  if (target_bias == 0)
  {
    left_direction = BACKWARD;
    right_direction = FORWARD;
    left_motor->set(255, left_direction);
    right_motor->set(255, right_direction);
    return;
  }

  if (target_speed >= 0)
  {
    left_direction = FORWARD;
    right_direction = FORWARD;
  }
  else
  {
    left_direction = BACKWARD;
    right_direction = BACKWARD;
    target_speed = abs(target_speed);
  }

  left_speed = target_speed;
  right_speed = cap((int)((float)target_speed * target_bias), 0, 255);
  // debug->log("SP:%d\t%d\t%d", left_speed1, left_speed, right_speed);
  left_motor->set(left_speed, left_direction);
  right_motor->set(right_speed, right_direction);
}

MotorDriver *MotorDriver::get_instance(void)
{
  if (instance == NULL)
  {
    if (left_motor == NULL)
    {
      debug->logl(ERROR, "trying to create MotorDriver before left_motor was set.");
    }
    else if (right_motor == NULL)
    {
      debug->logl(ERROR, "trying to create MotorDriver before right_motor was set.");
    }
    else if (odometry == NULL)
    {
      debug->logl(ERROR, "trying to create MotorDriver before odometry was set.");
    }
    instance = new MotorDriver();
  }
  return instance;
}