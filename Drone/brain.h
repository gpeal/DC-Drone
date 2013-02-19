#ifndef BRAIN_H
#define BRAIN_H

#include "debug.h"
#include "tracker.h"

class Brain
{
public:
  Brain();
  void loop(void);
private:
  Debug *debug;
  Tracker *tracker;
};

#endif