#include "Debug.h"
#include "Odometry.h"

Odometry::Odometry(Encoder left_encoder, Encoder right_encoder)
  :left_encoder(left_encoder),
   right_encoder(right_encoder)
{
  update_timer = new Metro(15);
}

void Odometry::loop(void)
{
  if (update_timer->check())
  {
    left_reading = left_encoder.read();
    right_reading = right_encoder.read();
    distance = (float)(left_reading + right_reading) / 2.0 * 0.096;
    heading = (float)(left_reading - right_reading) / 2.0;
    position.x = distance * sin(heading * 3.14 / 180);
    position.y = distance * cos(heading * 3.14 / 180);
    // Serial.print(distance);
    // Serial.print("\t");
    // Serial.print(heading);
    // Serial.print("\t");
    // Serial.print(left_reading);
    // Serial.print(",");
    // Serial.println(right_reading);
  }
}
