#ifndef ODOMETRY_H
#define ODOMETRY_H
#include <Encoder.h>
#include <Metro.h>

#define DISTANCE_CALIBRATION 3.57

class Odometry
{
public:
  void loop(void);
  Odometry(Encoder left_encoder, Encoder right_encoder);
  float x(void);
  float y(void);
private:
  Encoder left_encoder;
  Encoder right_encoder;
  Metro *update_timer;
  long left_reading;
  long right_reading;
  float distance;
  float heading;
};


#endif