#ifndef TRACKER_H
#define TRACKER_H

#include <Metro.h>
#include <Servo.h>
#include "Debug.h"

#define LEFT_EDGE 0
#define RIGHT_EDGE 1

#define SERVO_LEFT 0
#define SERVO_RIGHT 1

#define SERVO_MIN_POS 15
#define SERVO_MAX_POS 165
#define SERVO_MIN_SPEED 0.5
#define SERVO_MAX_SPEED 1.0

#define SERVO_SEARCHING_SPEED 1
#define SERVO_TRACKING_SPEED 0.05

class Tracker
{
  public:
    Tracker();
    Tracker(int _laser_pin, int _transistor_pin1, int _servo_pin1, int _transistor_pin2, int _servo_pin2);
    void loop(void);
  private:
    class Sensor
    {
    public:
      const int transistor_pin;
      Servo *servo;
      int state;
      float servo_pos;
      int servo_direction;
      int delta_threshold;
      // the threshold delta voltage in which the tracker will determine whether or not it is hitting a reflective object
      int last_reading;
      int last_delta;
      int last_found_pos;

      Sensor(int _servo_pin, int _transistor_pin, int edge);
    };

    // does one sweep of the servo to determine what the reading threshold should be
    void calibrate(void);
    void execute(void);
    void make_reading(Sensor *sensor);
    void move_servo(Sensor *sensor, int direction);
    void move_servo(Sensor *sensor, int direction, float amount);
    void reverse_servo(Sensor *sensor);
    int hit_prey(Sensor *sensor);

    const int laser_pin;

    int last_delta;
    Metro *execute_timer;
    Sensor *left_sensor;
    Sensor *right_sensor;
};

#endif