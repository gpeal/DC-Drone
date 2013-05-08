#include "Debug.h"
#include "MotorDriver.h"

MotorDriver::MotorDriver()
{
  timer = new Metro(200);
  average_left_speed_ra = new RunningAverage(RUNNING_AVERAGE_SIZE);
  average_right_speed_ra = new RunningAverage(RUNNING_AVERAGE_SIZE);
}

void MotorDriver::loop(void)
{
  long left_reading_diff, right_reading_diff;
  float seconds_diff;
  float current_left_speed, current_right_speed;
  int left_speed, right_speed;
  float left_speed_bias, right_speed_bias;
  if (!timer->check())
    return;

  seconds_diff = (float)((millis() - last_update_millis) / 1000.0);
  last_update_millis = millis();

  left_reading_diff = odometry->left_reading - last_left_reading;
  last_left_reading = odometry->left_reading;
  current_left_speed = abs((float)left_reading_diff / seconds_diff);
  average_left_speed_ra->addValue(current_left_speed);

  right_reading_diff = odometry->right_reading - last_right_reading;
  last_right_reading = odometry->right_reading;
  current_right_speed = abs((float)right_reading_diff / seconds_diff);
  average_right_speed_ra->addValue(current_right_speed);

  left_speed_bias = cap(target_right_speed / average_left_speed_ra->getAverage(), 0.98, 1.02);
  right_speed_bias = cap(target_right_speed / average_right_speed_ra->getAverage(), 098., 1.02);

  left_speed = cap((int)((float)left_motor->speed * left_speed_bias), 0, 255);
  left_motor->set(left_speed, left_motor->direction);
  right_speed = cap((int)((float)right_motor->speed * right_speed_bias), 0, 200);
  right_motor->set(right_speed, right_motor->direction);
  debug->log("SP%d\t%d\t%d\t%d", (int)(average_left_speed_ra->getAverage()), (int)(average_right_speed_ra->getAverage()), left_speed, right_speed);
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
  last_left_reading = odometry->left_reading;
  last_right_reading = odometry->right_reading;
  last_update_millis = millis();
  average_left_speed_ra->fillValue(target_left_speed, RUNNING_AVERAGE_SIZE);
  average_right_speed_ra->fillValue(target_right_speed, RUNNING_AVERAGE_SIZE);

  if (target_left_speed >= 0)
    left_direction = FORWARD;
  else
    left_direction = BACKWARD;

  if (target_right_speed >= 0)
    right_direction = FORWARD;
  else
    right_direction = BACKWARD;

  left_motor->set(target_left_speed * 100, left_direction);
  right_motor->set(target_right_speed * 100, right_direction);

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