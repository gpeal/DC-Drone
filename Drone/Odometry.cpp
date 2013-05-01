#include "Debug.h"
#include "Odometry.h"

Odometry::Odometry(Encoder left_encoder, Encoder right_encoder)
  :left_encoder(left_encoder),
   right_encoder(right_encoder)
{
  print_timer = new Metro(15);
}

void Odometry::loop(void)
{
  if (print_timer->check())
  {
    Serial.print(left_encoder.read());
    Serial.print("\t");
    Serial.println(right_encoder.read());
  }
}