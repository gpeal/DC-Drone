#ifndef MOTORDRIVER_H
#define MOTORDRIVER_H
#include <Metro.h>
#include "Motor.h"
#include "Odometry.h"

#define K_P 2
#define K_I 0.0
#define K_D 0.2

typedef struct EncoderReading
{
  long reading;
  long millis;
};

class MotorDriver
{
public:
  MotorDriver();
  void loop(void);
  void set(float target_left_speed, float target_right_speed);
  int target_speed;
  MotorDirection left_direction;
  MotorDirection right_direction;
  float target_left_speed;
  float target_right_speed;
  float left_error_sum;
  float right_error_sum;
  float last_left_error;
  float last_right_error;
  long last_update_millis;
  long last_left_reading;
  long last_right_reading;
  Metro *timer;
  static MotorDriver *get_instance(void);
  static Motor *left_motor;
  static Motor *right_motor;
  static Odometry *odometry;
private:
  static MotorDriver *instance;
};

#endif