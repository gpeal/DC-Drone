#include "Debug.h"
#include "Sensor.h"
#include "Tracker.h"

int Sensor::laser_pin;
int Sensor::laser_state;

Sensor::Sensor(int transistor_pin)
  :transistor_pin(transistor_pin)
{
  last_reading = 0;
  last_found_millis = -1;
  consecutive_hit_count = 0;
  // set this really high so it doesn't fire a hit during calibration
  delta_threshold = 9999;
  running_average = new RunningAverage(RUNNING_AVERAGE_SIZE);
}

/**
 * Sensor::calibrate does a full sweep from SEVO_MIN_POS to SERVO_MAX_POS
 *                   to gauge what a reasonable threshold would be for
 *                   determining when the laser hits prey
 *
 */
void Sensor::calibrate()
{
  delta_threshold = 15;
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

  running_average->addValue(last_delta);
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
  return running_average->getAverage() > delta_threshold;
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
  // int laser_value = digitalRead(laser_pin);
  if (laser_state == LOW)
  {
    laser_state = HIGH;
    digitalWrite(laser_pin, HIGH);
  }
  else
  {
    laser_state = LOW;
    digitalWrite(laser_pin, LOW);
  }
}