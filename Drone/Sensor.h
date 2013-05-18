#ifndef SENSOR_H
#define SENSOR_H

#include <Metro.h>
#include <Servo.h>
#include "RunningAverage.h"

#define CALIBRATION_SIZE 5
#define CALIBRATION_DEFAULT 999

#define LEFT_EDGE 0
#define RIGHT_EDGE 1

#define SERVO_LEFT 0
#define SERVO_RIGHT 1

#define SERVO_MIN_SPEED 0.5
#define SERVO_MAX_SPEED 1.0

#define SERVO_SEARCHING_SPEED 0.5
#define SERVO_TRACKING_SPEED 0.5

#define RUNNING_AVERAGE_SIZE 5

class Sensor
{
public:
  const int transistor_pin;
  int state;
  // the threshold delta voltage in which the tracker will determine whether or not it is hitting a reflective object
  int delta_threshold;
  int last_reading;
  int last_delta;
  unsigned long last_found_millis;
  // the laser oscillates around the edge. This tracks the left and right
  // range of the oscillation. Presumably, the edge actually exists at roughly
  // the center of the two
  int consecutive_hit_count;

  Sensor(int _transistor_pin);
  void make_reading(void);
  void calibrate(void);
  bool hit_prey(void);
  bool recently_hit_prey(void);
  // there should only be 1 laser pin for all of the sensors
  // the laser also needs to be accessible for the sensor in its calibration routine
  static int laser_state;
  static int laser_pin;
  static void toggle_laser(void);
  static void set_laser_pin(int pin);
private:
  RunningAverage *running_average;
};
#endif