#ifndef TRACKER_H
#define TRACKER_H

#include <Metro.h>
#include <Servo.h>
#include "Debug.h"
#include "Sensor.h"

#define TRACKER_STATE_NEITHER 0
#define TRACKER_STATE_RIGHT_ONLY 1
#define TRACKER_STATE_LEFT_ONLY 2
#define TRACKER_STATE_BOTH 3

class Tracker
{
  public:
    int state;

    Tracker();
    Tracker(int _transistor_pin1, int _servo_pin1, int _transistor_pin2, int _servo_pin2);
    void loop(void);
  private:
    // does one sweep of the servo to determine what the reading threshold should be
    void execute(void);
    void search(void);
    int last_delta;
    Metro *execute_timer;
    Sensor *left_sensor;
    Sensor *right_sensor;
};

#endif