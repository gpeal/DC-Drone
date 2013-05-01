#ifndef ODOMETRY_H
#define ODOMETRY_H
#include <Encoder.h>
#include <Metro.h>
#include "Utils.h"

#define DISTANCE_CALIBRATION 3.57

class Odometry
{
public:
  void loop(void);
  Odometry(Encoder left_encoder, Encoder right_encoder);
  Coordinate position;
private:
  float x(void);
  float y(void);
  Encoder left_encoder;
  Encoder right_encoder;
  Metro *update_timer;
  long left_reading;
  long right_reading;
  float distance;
  float heading;
};


#endif