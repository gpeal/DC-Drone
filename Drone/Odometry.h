#ifndef ODOMETRY_H
#define ODOMETRY_H
#include <Encoder.h>
#include <Metro.h>

class Odometry
{
public:
  void loop(void);
  Odometry(Encoder left_encoder, Encoder right_encoder);
private:
  Encoder left_encoder;
  Encoder right_encoder;
  Metro *print_timer;
};


#endif