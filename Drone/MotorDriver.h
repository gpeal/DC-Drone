#ifndef MOTORDRIVER_H
#define MOTORDRIVER_H
#include <Metro.h>
#include "Motor.h"
#include "Odometry.h"

#define HISTORY_SIZE 5

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
  void set(int target_speed, float target_bias);
  int target_speed;
  MotorDirection left_direction;
  MotorDirection right_direction;
  long starting_left_reading;
  long starting_right_reading;
  float target_bias;
  float bias;
  Metro *timer;
  static MotorDriver *get_instance(void);
  static Motor *left_motor;
  static Motor *right_motor;
  static Odometry *odometry;
private:
  static MotorDriver *instance;
};

#endif