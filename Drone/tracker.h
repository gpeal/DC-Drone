#ifndef TRACKER_H
#define TRACKER_H

#include <Servo.h>

#include "Debug.h"

#define LEFT_EDGE 0
#define RIGHT_EDGE 1

#define ONE_EDGE 0
#define TWO_EDGE 1

class Tracker
{
  public:
    Tracker();
    Tracker(int _mode, int _edge);
    void loop(void);
  private:
    // determines whether there is one laser tracking one edge or two lasers tracking both
    int mode;
    // determines which edge is being tracked
    int edge;

    int pos;
    Debug *debug;
    Servo *servo;
};

#endif