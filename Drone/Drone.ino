#include <Servo.h>

#include "brain.h"

Brain *brain;

void setup()
{
  brain = new Brain();
}

void loop()
{
  brain->loop();
}