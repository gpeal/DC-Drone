#include "Debug.h"
#include "Sensor.h"
#include "Tracker.h"

Sensor::Sensor(int _servo_pin, int _transistor_pin, int edge)
  : transistor_pin(_transistor_pin)
{
  servo = new Servo();
  servo->attach(_servo_pin);
  pos = SERVO_MIN_POS;
  pos_ra = new RunningAverage(50);
  servo->write(pos);
  direction = !edge;
  last_reading = 0;
  last_found_pos = (int)((SERVO_MAX_POS - SERVO_MIN_POS) / 2);
  last_found_millis = -1;
  delta_threshold = 40;
  debug->log("Initializing Tracker");
}

/**
 * Sensor::calibrate does a full sweep from SEVO_MIN_POS to SERVO_MAX_POS
 *                   to gauge what a reasonable threshold would be for
 *                   determining when the laser hits prey
 *
 */
void Sensor::calibrate()
{
  // int deltas[CALIBRATION_SIZE];
  // int delta;
  // int number_of_valid_readings = 0;
  // int sum_of_valid_readings = 0;
  // Metro calibration_timer(15);

  // for (int i = 0; i < CALIBRATION_SIZE; i++)
  // {
  //   deltas[i] = CALIBRATION_DEFAULT;
  // }

  // while(pos <= SERVO_MAX_POS)
  // {
  //   if (calibration_timer.check())
  //   {
  //       make_reading();
  //       Sensor::toggle_laser();
  //       for (int i = 0; i < CALIBRATION_SIZE; i++)
  //       {
  //         if (last_delta < deltas[i] && last_delta >= 1)
  //         {
  //           deltas[i] = last_delta;
  //           break;
  //         }
  //       }
  //       debug->log("Servo Pos: %d, %d", (int)pos, last_delta);
  //       debug->log("Min Deltas: [%d, %d, %d, %d, %d]", deltas[0], deltas[1], deltas[2], deltas[3], deltas[4]);
  //       move_servo(SERVO_RIGHT, 3);
  //   }
  // }
  // // find the average valid reading
  // for (int i = 0; i < CALIBRATION_SIZE; i++)
  // {
  //   if (deltas[i] != CALIBRATION_DEFAULT)
  //   {
  //     number_of_valid_readings++;
  //     sum_of_valid_readings += deltas[i];
  //   }
  // }
  // delta_threshold = 4 * sum_of_valid_readings / number_of_valid_readings;
  delta_threshold = 4;
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
    last_found_pos = pos;
    last_found_millis = millis();
  }
  pos_ra->addValue(pos);
}

void Sensor::move_servo(int direction, float amount)
{
  int adjusted_pos;
  if (direction == SERVO_LEFT)
  {
    pos -= amount;
  }
  else if (direction == SERVO_RIGHT)
  {
    pos += amount;
  }
  // debug->log("Pos: %d %d", (int)pos, (int)amount);

  // in reality, 180 is on the left and 0 is on the right which is reverse of what it
  // logically should be so reverse it here
  adjusted_pos = 180 - pos;
  servo->writeMicroseconds(1000.0 + (1000.0/180.0) * adjusted_pos);
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
  return abs(pos - pos_ra->getAverage()) < RECENT_HIT_POS_THRESHOLD;
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