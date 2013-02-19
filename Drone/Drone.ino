#include <Servo.h>

#include "brain.h"

Brain brain;

void setup()
{
  brain = Brain();

}

void loop()
{
  brain.loop();
}