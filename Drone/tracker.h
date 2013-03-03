#ifndef TRACKER_H
#define TRACKER_H

#include <Servo.h>

#include "Debug.h"
#include "Metro.h"

#define LEFT_EDGE 0
#define RIGHT_EDGE 1

#define ONE_EDGE 0
#define TWO_EDGE 1

#define SERVO_LEFT 0
#define SERVO_RIGHT 1

#define SERVO_MIN_POS 15
#define SERVO_MAX_POS 165
#define SERVO_MIN_SPEED 0.5
#define SERVO_MAX_SPEED 1.0

class Tracker
{
  public:
    Tracker();
    Tracker(int _mode, int _edge_mode, int _laser_pin, int _transistor_pin);
    void loop(void);
  private:
    typedef struct
    {
      Servo *servo;

      int state;

      float servo_pos;
      int servo_direction;
      // the threshold delta voltage in which the tracker will determine whether or not it is hitting a reflective object
      int last_reading;
      int last_found_pos;
    }Sensor;

    // does one sweep of the servo to determine what the reading threshold should be
    void calibrate(void);
    void execute(void);
    int make_reading(Sensor *sensor);
    void move_servo(Sensor *sensor, int direction);
    void move_servo(Sensor *sensor, int direction, float amount);
    void reverse_servo(Sensor *sensor);

    // determines whether there is one laser tracking one edge or two lasers tracking both
    const int edge_mode;
    // determines which edge is being tracked
    const int edge;

    const int laser_pin;
    const int transistor_pin;

    int reading_threshold;
    Metro *execute_timer;
    Sensor *left_sensor;
    Sensor *right_sensor;
};

#endif