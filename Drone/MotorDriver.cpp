#include "Debug.h"
#include "MotorDriver.h"

MotorDriver::MotorDriver()
{
  EncoderReading left_reading;
  EncoderReading right_reading;
  left_reading.reading = odometry->left_reading;
  left_reading.millis = millis();
  right_reading.reading = odometry->right_reading;
  right_reading.millis = millis();

  // this determines how many readings back the current encoder
  // reading should be compared against to calculate velocity
  for(int i = 0; i < HISTORY_SIZE; i++)
  {
    left_encoder_history[i] = left_reading;
    right_encoder_history[i] = right_reading;
  }

  target_left_velocity = 0;
  target_right_velocity = 0;

  timer = new Metro(100);
}

void MotorDriver::loop(void)
{
  if (!timer->check())
    return;

  float left_velocity, right_velocity;
  MotorDirection left_direction, right_direction;
  int left_speed, right_speed;

  // update left reading
  EncoderReading left_reading;
  left_reading.reading = odometry->left_reading;
  left_reading.millis = millis();
  for(int i = 0; i < HISTORY_SIZE - 1; i++)
  {
    left_encoder_history[i] = left_encoder_history[i + 1];
  }
  left_encoder_history[HISTORY_SIZE - 1] = left_reading;

  // update right reading
  EncoderReading right_reading;
  right_reading.reading = odometry->right_reading;
  right_reading.millis = millis();
  for(int i = 0; i < HISTORY_SIZE - 1; i++)
  {
    right_encoder_history[i] = right_encoder_history[i + 1];
  }
  right_encoder_history[HISTORY_SIZE - 1] = right_reading;

  // calculate velocity from reading
  left_velocity = (float)(left_encoder_history[HISTORY_SIZE - 1].reading -
                          left_encoder_history[0].reading) /
                  (float)(left_encoder_history[HISTORY_SIZE - 1].millis -
                          left_encoder_history[0].millis) * 10000.0;
  right_velocity = (float)(right_encoder_history[HISTORY_SIZE - 1].reading -
                          right_encoder_history[0].reading) /
                  (float)(right_encoder_history[HISTORY_SIZE - 1].millis -
                          right_encoder_history[0].millis) * 10000.0;
  if (target_left_velocity >= 0)
    left_direction = CCW;
  else
    left_direction = CW;
  if (target_right_velocity >= 0)
    right_direction = CCW;
  else
    right_direction = CW;

  // left_speed = (int)cap(abs(target_left_velocity / left_velocity) * (float)left_motor->speed, 0.0, 255.0);
  // right_speed = (int)cap(abs(target_right_velocity / right_velocity) * (float)right_motor->speed, 0.0, 255.0);
  // prevent dividing by zero
  if (abs(left_velocity) < 1)
    left_velocity = 1 * target_left_velocity / abs(target_left_velocity);
  if (abs(right_velocity) < 1)
    right_velocity = 1 * target_right_velocity / abs(target_right_velocity);
  left_speed = cap(left_motor->speed + 20 * (target_left_velocity / left_velocity - 1), 0.0, 255.0);
  right_speed = cap(right_motor->speed + 20 * (target_right_velocity / right_velocity - 1), 0.0, 255.0);
  debug->log("V:%d\t%d\t%d\t%d", (int)(left_velocity), (int)(right_velocity), (int)left_speed, (int)right_speed);

  left_motor->set(left_speed, left_direction);
  right_motor->set(right_speed, right_direction);
}

/**
 * MotorDriver::set sets the desired velocity of each tread
 * float target_left_velocity: the desired velocity of the left tread
 * float target_right_velocity: the desired velocity of the right tread
 *
 * negative values turns the tread backwards
 * a good velocity is on the order of 300
 */
void MotorDriver::set(float target_left_velocity, float target_right_velocity)
{
  MotorDirection left_direction, right_direction;

  this->target_left_velocity = target_left_velocity;
  this->target_right_velocity = target_right_velocity;

  if (target_left_velocity >= 0)
    left_direction = CCW;
  else
    left_direction = CW;
  if (target_right_velocity >= 0)
    right_direction = CCW;
  else
    right_direction = CW;
  // TODO pick a better number for this
  left_motor->set(50, left_direction);
  right_motor->set(50, right_direction);
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