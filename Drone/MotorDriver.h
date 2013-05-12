#ifndef MOTORDRIVER_H
#define MOTORDRIVER_H
#include <Metro.h>
#include "Motor.h"

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
  Metro *timer;
  static MotorDriver *get_instance(void);
  static Motor *left_motor;
  static Motor *right_motor;
private:
  static MotorDriver *instance;
};

#endif