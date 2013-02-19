#ifndef DEBUG_H
#define DEBUG_H

#define INFO "INFO: "
#define WARN "WARN: "

class Debug
{
  public:
    Debug();
    // log msg back to computer but default to INFO level
    void log(char *msg);

    void logl(char *level, char *msg);
};

#endif

