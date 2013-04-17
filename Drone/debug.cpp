#include "Debug.h"

Debug::Debug()
{
  Serial.begin(9600);
}

void Debug::log(char *fmt, ...)
{
  if(debug == NULL)
  {
    debug = new Debug();
  }
  va_list argList;
  char buffer[64] = { 0 };
  va_start(argList, fmt);
  vsprintf(buffer, fmt, argList);
  va_end(argList);
  Serial.print(INFO);
  Serial.println(buffer);
  va_end(argList);
}

void Debug::logl(char *level, char *fmt, ...)
{
  if(debug == NULL)
  {
    debug = new Debug();
  }
  va_list argList;
  char buffer[64] = { 0 };
  va_start(argList, fmt);
  vsprintf(buffer, fmt, argList);
  va_end(argList);
  Serial.print(level);
  Serial.println(buffer);
  va_end(argList);
}