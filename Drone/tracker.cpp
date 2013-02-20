#include "Arduino.h"
#include "Tracker.h"
#include <stdlib.h>

Tracker::Tracker(int _mode, int _edge)
{
  mode = _mode;
  edge = _edge;
  debug = new Debug();
  servo = new Servo();
  servo->attach(9);
  debug->log("Attaching servo to port 9");
}

void Tracker::loop(void)
{
  char out_buffer[100];

  for(pos = 0; pos < 180; pos += 20)  // goes from 0 degrees to 180 degrees
    {
      itoa(pos, out_buffer, 10);                      // in steps of 1 degree
      debug->log(out_buffer);
      servo->write(pos);            // tell servo to go to position in variable 'pos'
      delay(1000);                       // waits 15ms for the servo to reach the position
    }
    for(pos = 180; pos>=1; pos-=20)     // goes from 180 degrees to 0 degrees
    {
      itoa(pos, out_buffer, 10);                      // in steps of 1 degree
      debug->log(out_buffer);
      servo->write(pos);          // tell servo to go to position in variable 'pos'
      delay(1000);                       // waits 15ms for the servo to reach the position
    }
}