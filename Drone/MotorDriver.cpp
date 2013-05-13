#include "Debug.h"
#include "MotorDriver.h"

Motor *MotorDriver::left_motor;
Motor *MotorDriver::right_motor;
MotorDriver *MotorDriver::instance;

MotorDriver::MotorDriver()
{
  timer = new Metro(200);
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
void MotorDriver::set(int left_speed, int right_speed)
{
  MotorDirection left_direction, right_direction;
  if (left_speed >= 0)
    left_direction = FORWARD;
  else
    left_direction = BACKWARD;

  if (right_speed >= 0)
    right_direction = FORWARD;
  else
    right_direction = BACKWARD;

  left_motor->set(abs(left_speed), left_direction);
  right_motor->set(abs(right_speed), right_direction);

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
    instance = new MotorDriver();
  }
  return instance;
}