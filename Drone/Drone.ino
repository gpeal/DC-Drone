#include <Servo.h>

#include "Debug.h"
#include "Tracker.h"

Tracker *tracker;

void setup()
{
  tracker = new Tracker(ONE_EDGE, LEFT_EDGE, 2);
  debug->log("Starting UP");
}

void loop()
{
  tracker->loop();
}