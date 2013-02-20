#include "Arduino.h"
#include "Periodic.h"

Periodic::Periodic(void *_self, void (*_callback)())
  : self(_self),
    callback(_callback)
{
}

void Periodic::loop(void)
{
  static_cast<Periodic *>(self)->callback();
  delay(1000);
}