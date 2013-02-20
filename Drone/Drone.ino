#include <Servo.h>

#include "Debug.h"
#include "Tracker.h"

Debug *debug;
Tracker *tracker;

void setup()
{
  tracker = new Tracker(ONE_EDGE, LEFT_EDGE);
  debug = new Debug();
}

void loop()
{
  tracker->loop();
}