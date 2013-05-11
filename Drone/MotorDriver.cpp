#include "Debug.h"
#include "MotorDriver.h"

MotorDriver::MotorDriver()
{
  timer = new Metro(200);
}

void MotorDriver::loop(void)
{
  return;



  // long left_reading_diff, right_reading_diff;
  // float seconds_diff;
  // float current_left_speed, current_right_speed;
  // int left_speed, right_speed;
  // float left_error, right_error;
  // float d_left_error, d_right_error;
  // int left_bias, right_bias;
  // if (!timer->check())
  //   return;

  // seconds_diff = (float)((millis() - last_update_millis) / 1000.0);
  // last_update_millis = millis();

  // left_reading_diff = odometry->left_reading - last_left_reading;
  // last_left_reading = odometry->left_reading;
  // current_left_speed = abs((float)left_reading_diff / seconds_diff);

  // right_reading_diff = odometry->right_reading - last_right_reading;
  // last_right_reading = odometry->right_reading;
  // current_right_speed = abs((float)right_reading_diff / seconds_diff);

  // left_error = target_left_speed - current_left_speed;
  // right_error = target_right_speed - current_right_speed;

  // d_left_error = left_error - last_left_error;
  // d_right_error = right_error - last_right_error;

  // last_left_error = left_error;
  // last_right_error = right_error;

  // left_bias = K_P * left_error + K_D * d_left_error;
  // right_bias = K_P * right_error + K_D * d_right_error;

  // left_speed = cap(left_motor->speed + left_bias, 0, 255);
  // right_speed = cap(right_motor->speed + right_bias, 0, 255);


  // left_motor->set(left_speed, left_motor->direction);
  // right_motor->set(right_speed, right_motor->direction);
  // debug->log("SP%d\t%d\t%d\t%d", (int)(current_left_speed), (int)(current_right_speed), left_speed, right_speed);
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
void MotorDriver::set(float target_left_speed, float target_right_speed)
{
  MotorDirection left_direction, right_direction;
  this->target_left_speed = target_left_speed;
  this->target_right_speed = target_right_speed;
  last_update_millis = millis();

  left_error_sum = 0;
  right_error_sum = 0;

  last_left_error = 0;
  last_right_error = 0;

  if (target_left_speed >= 0)
    left_direction = FORWARD;
  else
    left_direction = BACKWARD;

  if (target_right_speed >= 0)
    right_direction = FORWARD;
  else
    right_direction = BACKWARD;

  left_motor->set((int)abs(target_left_speed), left_direction);
  right_motor->set((int)abs(target_right_speed), right_direction);

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