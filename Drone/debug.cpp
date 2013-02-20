#include "Arduino.h"
#include "Debug.h"

Debug::Debug()
{
  Serial.begin(9600);
}

void Debug::log(char *msg)
{
  logl(INFO, msg);
}

void Debug::logl(char *level, char *msg)
{
  if(debug == NULL)
  {
    debug = new Debug();
  }
  Serial.print(level);
  Serial.println(msg);
}