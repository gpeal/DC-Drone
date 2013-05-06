#include "Debug.h"
#include "Odometry.h"

Odometry::Odometry(Encoder left_encoder, Encoder right_encoder)
  :left_encoder(left_encoder),
   right_encoder(right_encoder)
{
  update_timer = new Metro(5);
  last_left_reading = left_encoder.read();
  last_right_reading = right_encoder.read();
  heading = 0;
  distance = 0;
  position.x = 0;
  position.y = 0;
}

void Odometry::loop(void)
{
  int d_left, d_right, d_distance;
  float left_inches, right_inches, inches, d_heading;
  if (update_timer->check())
  {
    left_reading = left_encoder.read();
    right_reading = right_encoder.read();

    d_left = left_reading - last_left_reading;
    d_right = right_reading - last_right_reading;

    last_left_reading = left_reading;
    last_right_reading = right_reading;

    left_inches = (float)d_left/LEFT_CLICKS_PER_INCH;
    right_inches = (float)d_right/RIGHT_CLICKS_PER_INCH;
    d_distance = (left_inches + right_inches) / 2.0;

    d_heading = left_inches - right_inches;
    heading += d_heading / WHEEL_BASE;
    // heading -= (float)((int)(heading/TWOPI))*TWOPI;

    // position.x += d_distance * cos(heading);
    // position.y += d_distance * sin(heading);
    distance += d_distance;


    // Serial.print(left_reading);
    // Serial.print("\t");
    // Serial.println(right_reading);
    // Serial.print("\t");
    // Serial.print(distance);
    // Serial.print("\t");
    // Serial.println(heading * 180 / PI);
    // Serial.print("\t");
    // Serial.println(heading);
    // Serial.print("\t");
    // Serial.print(position.x);
    // Serial.print(",");
    // Serial.println(position.y);
  }
}