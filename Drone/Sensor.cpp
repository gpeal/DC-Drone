#include "Debug.h"
#include "Sensor.h"
#include "Tracker.h"

Sensor::Sensor(int transistor_pin)
  :transistor_pin(transistor_pin)
{
  last_reading = 0;
  last_found_millis = -1;
  consecutive_hit_count = 0;
  // set this really high so it doesn't fire a hit during calibration
  delta_threshold = 9999;
}

/**
 * Sensor::calibrate does a full sweep from SEVO_MIN_POS to SERVO_MAX_POS
 *                   to gauge what a reasonable threshold would be for
 *                   determining when the laser hits prey
 *
 */
void Sensor::calibrate()
{
  int delta_sum = 0;
  make_reading();
  for(int i = 0; i < 50; i++)
  {
    toggle_laser();
    make_reading();
    delta_sum += abs(last_delta);
    delay(10);
  }

  delta_threshold = (int)(1.8 * (float)delta_sum / 20);

  debug->log("Delta threshold: %d", delta_threshold);
}

/**
 * Sensor::make_reading updates the recent delta values for the sensor
 *
 */
void Sensor::make_reading(void)
{
  int laser_value = digitalRead(Sensor::laser_pin);
  int reading = analogRead(transistor_pin);
  last_delta = reading - last_reading;
  last_reading = reading;
  // laser is off, invert the delta because we are comparing the change from the
  // previous value which should be higher if the laser was on
  if(laser_value == LOW)
  {
    last_delta *= -1;
  }

  if (hit_prey())
  {
    last_found_millis = millis();
    consecutive_hit_count++;
  }
  else
  {
    consecutive_hit_count = 0;
  }
}

/**
 * Sensor::hit_prey  returns whether the sensor hit a prey on its last reading
 *
 * @return 0 or 1 whether or not it hit a prey
 */
bool Sensor::hit_prey(void)
{
  return last_delta > delta_threshold;
}

/**
 * Sensor::recently_hit_prey
 *
 * @return whether or not the sensor has hit the prey within RECENT_HIT_TIME_THRESHOLD millis
 */
bool Sensor::recently_hit_prey(void)
{
  return consecutive_hit_count > 3;
}

void Sensor::set_laser_pin(int pin)
{
  laser_pin = pin;
  pinMode(laser_pin, OUTPUT);
}

void Sensor::toggle_laser(void)
{
  if (laser_pin == -1)
  {
    debug->logl(ERROR, "Laser toggled before being set");
    return;
  }
  int laser_value = digitalRead(laser_pin);
  digitalWrite(laser_pin, !laser_value);
}