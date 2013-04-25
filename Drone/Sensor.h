#ifndef SENSOR_H
#define SENSOR_H

#include <Metro.h>
#include <Servo.h>

#define RECENT_HIT_TIME_THRESHOLD 200
#define CALIBRATION_SIZE 5
#define CALIBRATION_DEFAULT 999

#define LEFT_EDGE 0
#define RIGHT_EDGE 1

#define SERVO_LEFT 0
#define SERVO_RIGHT 1

#define SERVO_MIN_POS 15
#define SERVO_MAX_POS 165
#define SERVO_MIN_SPEED 0.5
#define SERVO_MAX_SPEED 1.0

#define SERVO_SEARCHING_SPEED 1
#define SERVO_TRACKING_SPEED 0.5

class Sensor
{
public:
  static int a;
  const int transistor_pin;
  Servo *servo;
  int state;
  float pos;
  int direction;
  int delta_threshold;
  // the threshold delta voltage in which the tracker will determine whether or not it is hitting a reflective object
  int last_reading;
  int last_delta;
  int last_found_pos;
  unsigned long last_found_millis;
  // the laser oscillates around the edge. This tracks the left and right
  // range of the oscillation. Presumably, the edge actually exists at roughly
  // the center of the two
  float oscillation_left_edge;
  float oscillation_right_edge;

  Sensor(int _servo_pin, int _transistor_pin, int edge);
  void move_servo(int direction, float amount);
  void make_reading(void);
  void calibrate(void);
  bool hit_prey(void);
  bool recently_hit_prey(void);
  // there should only be 1 laser pin for all of the sensors
  // the laser also needs to be accessible for the sensor in its calibration routine
  static int laser_pin;
  static void toggle_laser(void);
  static void set_laser_pin(int pin);

};
#endif