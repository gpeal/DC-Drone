#ifndef MOTORDRIVER_H
#define MOTORDRIVER_H
#include <Metro.h>
#include "Motor.h"
#include "Odometry.h"
#include "RunningAverage.h"

#define RUNNING_AVERAGE_SIZE 3

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
  long last_left_reading;
  long last_right_reading;
  long last_update_millis;
  Metro *timer;
  static MotorDriver *get_instance(void);
  static Motor *left_motor;
  static Motor *right_motor;
  static Odometry *odometry;
private:
  RunningAverage *average_left_speed_ra;
  RunningAverage *average_right_speed_ra;
  static MotorDriver *instance;
};

#endif