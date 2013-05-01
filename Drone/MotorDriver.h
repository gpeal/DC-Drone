#ifndef MOTORDRIVER_H
#define MOTORDRIVER_H
#include "Motor.h"
#include "Odometry.h"
#include "Utils.h"

// the max speed (0-1)
#define SPEED 0.5

class MotorDriver
{
public:
  MotorDriver(Motor *left_motor, Motor *right_motor, Odometry *odometry);
  void loop(void);
  void move_to(float x, float y);
private:
  float heading_to_target(void);
  Motor *left_motor;
  Motor *right_motor;
  Odometry *odometry;
  Coordinate target;
  Metro *timer;
};

#endif