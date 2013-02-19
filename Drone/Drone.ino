#include "debug.h"

Debug debug;

void setup()
{
  debug = Debug();

}

void loop()
{
  debug.log("log");
  debug.logl(WARN, "warn");
  delay(1000);
}