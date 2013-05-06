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
  void set(float target_left_velocity, float target_right_velocity);
  // keep the last n times the encoder changed in a queue
  // compare the nth most recent recording to the most recent
  // recording to calculate velocity
  EncoderReading left_encoder_history[HISTORY_SIZE];
  EncoderReading right_encoder_history[HISTORY_SIZE];
  float target_left_velocity;
  float target_right_velocity;
  Metro *timer;
  static MotorDriver *get_instance(void);
  static Motor *left_motor;
  static Motor *right_motor;
  static Odometry *odometry;
private:
  static MotorDriver *instance;
};

#endif