#ifndef SONAR_H
#define SONAR_H

#include <Metro.h>
#include <NewPing.h>

namespace Sonar
{
  void loop(void);
  extern Metro *sonar_timer;
  extern NewPing *prey_sonar;
  extern NewPing *nest_sonar;
  extern float prey_inches;
  extern float nest_inches;
}

#endif