#ifndef TRACKER_H
#define TRACKER_H

#include <Metro.h>
#include <Servo.h>
#include "Debug.h"
#include "MotorDriver.h"
#include "Sensor.h"

#define TRACKER_STATE_NONE 0
#define TRACKER_STATE_RIGHT 1
#define TRACKER_STATE_MIDDLE 2
#define TRACKER_STATE_MIDDLE_RIGHT 3
#define TRACKER_STATE_LEFT 4
#define TRACKER_STATE_LEFT_RIGHT 5
#define TRACKER_STATE_LEFT_MIDDLE 6
#define TRACKER_STATE_LEFT_MIDDLE_RIGHT 7

class Tracker
{
  public:
    Tracker(int transistor_pin_left, int transistor_pin_middle, int transistor_pin_right);
    Tracker(int transistor_pin_left, int transistor_pin_right);
    bool loop(void);
    void init(void);
    int state;
    Sensor *left_sensor;
    Sensor *right_sensor;
    Sensor *middle_sensor;
  private:
    // does one sweep of the servo to determine what the reading threshold should be
    void search(void);
    int last_delta;
    Metro *execute_timer;
};

#endif