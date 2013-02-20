#ifndef TRACKER_H
#define TRACKER_H

#include <Servo.h>

#include "Debug.h"
#include "Timer.h"

#define LEFT_EDGE 0
#define RIGHT_EDGE 1

#define ONE_EDGE 0
#define TWO_EDGE 1

class Tracker
{
  public:
    Tracker();
    Tracker(int _mode, int _edge_mode, int _laser_pin, int _transistor_pin);
    void loop(void);
  private:
    // void move_servo(void);
    void toggle_laser(void);
    static void toggle_laser_wrapper(void *self);
    // determines whether there is one laser tracking one edge or two lasers tracking both
    const int edge_mode;
    // determines which edge is being tracked
    const int edge;
    const int laser_pin;
    const int transistor_pin;

    int pos;
    Servo *servo;
    Timer *laser_timer;

    int last_transistor_reading;
};

#endif