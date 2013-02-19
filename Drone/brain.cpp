#include "brain.h"

Brain::Brain()
{
  tracker = new Tracker(ONE_EDGE, LEFT_EDGE);
}

void Brain::loop(void)
{
  tracker->loop();
}