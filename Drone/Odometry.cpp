#include "Debug.h"
#include "Odometry.h"

Odometry::Odometry(Encoder left_encoder, Encoder right_encoder)
  :left_encoder(left_encoder),
   right_encoder(right_encoder)
{
  left_encoder.write(0);
  right_encoder.write(0);
  update_timer = new Metro(100);
  left_reading = left_encoder.read();
  last_left_reading = left_reading;
  right_reading = right_encoder.read();
  last_right_reading = right_reading;
  heading = 0;
  distance = 0;
  position.x = 0;
  position.y = 0;
}

void Odometry::loop(void)
{
  int d_left, d_right;
  float left_inches, right_inches, inches, d_heading, d_distance;
  // these must be called regularly
  left_reading = left_encoder.read();
  right_reading = right_encoder.read();
  if (update_timer->check())
  {

    d_left = left_reading - last_left_reading;
    d_right = right_reading - last_right_reading;

    last_left_reading = left_reading;
    last_right_reading = right_reading;

    left_inches = (float)d_left/LEFT_CLICKS_PER_INCH;
    right_inches = (float)d_right/RIGHT_CLICKS_PER_INCH;

    d_heading = left_inches - right_inches;
    heading += d_heading / WHEEL_BASE;
    // heading -= (float)((int)(heading/360))*360;

    d_distance = (left_inches + right_inches) / 2.0;
    position.x += d_distance * cos(heading * PI / 180.0);
    position.y += d_distance * sin(heading * PI / 180.0);
    distance += d_distance;

    debug->log("E%d\t%d", left_reading, right_reading);
    // debug->log("R%d\t%d\t%d", (int)(position.x), (int)(position.y), (int)(heading));
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