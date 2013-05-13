#include "Debug.h"
#include "Sonar.h"

Metro *Sonar::sonar_timer = new Metro(50);
NewPing *Sonar::prey_sonar;
NewPing *Sonar::nest_sonar;
float Sonar::prey_inches = 0;
float Sonar::nest_inches = 0;

void Sonar::loop(void)
{
  if (!sonar_timer->check())
  {
    return;
  }

  if (prey_sonar != NULL)
  {
    prey_inches = (float)prey_sonar->ping() / (float)US_ROUNDTRIP_IN;
  }
  if (nest_sonar != NULL)
  {
    nest_inches = (float)nest_sonar->ping() / (float)US_ROUNDTRIP_IN;
  }
}