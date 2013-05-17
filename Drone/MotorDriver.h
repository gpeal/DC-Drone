#ifndef MOTORDRIVER_H
#define MOTORDRIVER_H
#include <Metro.h>
#include "Motor.h"

#define RIGHT 1
#define LEFT -1

typedef struct EncoderReading
{
  long reading;
  long millis;
};

class MotorDriver
{
public:
  MotorDriver();
  void set(int left_speed, int right_speed);
  void loop(void);
  void spin(int duty, int spin_direction);
  void spin(int min_duty, int max_duty, int ramp_up_time, int spin_direction);
  Metro *timer;
  int spinning;
  long spinning_millis;
  int min_duty;
  int max_duty;
  int ramp_up_time;
  int spin_direction;
  static MotorDriver *get_instance(void);
  static Motor *left_motor;
  static Motor *right_motor;
private:
  static MotorDriver *instance;
};

#endif