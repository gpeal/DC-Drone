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
#define PREY_POSITION_BUFFER_SIZE 5

class Tracker
{
  public:
    int state;

    Tracker();
    Tracker(int _transistor_pin_left, int _servo_pin_left, int _transistor_pin_right, int _servo_pin_right);
    void loop(void);
    void update_prey_position(void);
    float prey_position;
    Sensor *left_sensor;
    Sensor *right_sensor;
  private:
    // does one sweep of the servo to determine what the reading threshold should be
    void execute(void);
    void search(void);
    int last_delta;
    Metro *execute_timer;
    float prey_position_buffer[PREY_POSITION_BUFFER_SIZE];
    int prey_position_buffer_i;
};

#endif