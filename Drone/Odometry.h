#ifndef ODOMETRY_H
#define ODOMETRY_H
#include <Encoder.h>
#include <Metro.h>
#include "Utils.h"

#define WHEEL_BASE 11.86f
#define LEFT_CLICKS_PER_INCH 4.6666f
#define RIGHT_CLICKS_PER_INCH 4.6666f
#define PI 3.14159f
#define TWOPI 2.0f * PI

class Odometry
{
public:
  void loop(void);
  Odometry(Encoder left_encoder, Encoder right_encoder);
  Coordinate position;
  float heading;
  float distance;
private:
  float x(void);
  float y(void);
  Encoder left_encoder;
  Encoder right_encoder;
  Metro *update_timer;
  long last_left_reading;
  long last_right_reading;
  long left_reading;
  long right_reading;
};


#endif