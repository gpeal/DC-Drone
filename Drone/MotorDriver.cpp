#include "Debug.h"
#include "MotorDriver.h"
#include "Utils.h"

Motor *MotorDriver::left_motor;
Motor *MotorDriver::right_motor;
MotorDriver *MotorDriver::instance;

MotorDriver::MotorDriver()
{
  timer = new Metro(200);
  spinning = false;
}

void MotorDriver::loop(void)
{
  int duty, speed;
  MotorDirection left_direction, right_direction;
  if (spinning == false)
    return;

  duty =  (float)(millis() - spinning_millis) / 1000.0 * (float)(max_duty - min_duty) / (float)ramp_up_time + min_duty;
  duty = cap(duty, min_duty, max_duty);
  if (millis() % 100 < duty)
  {
    speed = 255;
  }
  else
  {
    speed = 0;
  }
  if (spin_direction == RIGHT)
  {
    left_direction = FORWARD;
    right_direction = BACKWARD;
  }
  else
  {
    left_direction = BACKWARD;
    right_direction = FORWARD;
  }
  left_motor->set(speed, left_direction);
  right_motor->set(speed, right_direction);
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
  spinning = false;
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

void MotorDriver::spin(int duty, int spin_direction)
{
  spin(duty, duty, 1, spin_direction);
}

void MotorDriver::spin(int min_duty, int max_duty, int ramp_up_time, int spin_direction)
{
  if (this->min_duty == min_duty && this->max_duty == max_duty && this->ramp_up_time == ramp_up_time && this->spin_direction == spin_direction)
    return;

  this->min_duty = min_duty;
  this->max_duty = max_duty;
  this->ramp_up_time = ramp_up_time;
  this->spin_direction = spin_direction;
  spinning = true;
  spinning_millis = millis();
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
