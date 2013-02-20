#ifndef PERIODIC_H
#define PERIODIC_H

/*
 * This library creates a set of functions that can be written by any object
 * to make a periodic function.
 *
 * Member functions can't be used as callbacks to most timer libraries since
 * they can't be called without their `this` object which the libraries don't
 * expect to need.
 */

class Periodic
{
public:
  Periodic(void *_self, void (*_callback)());
  void loop(void);
  void run_every_1s(void);

private:
  void *self;
  void (*callback)();
};

#endif