#ifndef DEBUG_H
#define DEBUG_H

#include "Arduino.h"

#define INFO "INFO: "
#define WARN "WARN: "
#define ERROR "ERROR: "

class Debug
{
  public:
    Debug();
    // log msg back to computer but default to INFO level
    void log(char *fmt, ...);

    void logl(char *level, char *fmt, ...);
};

static Debug *debug = NULL;

#endif

