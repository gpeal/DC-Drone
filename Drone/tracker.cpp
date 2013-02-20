#include "Arduino.h"
#include "Tracker.h"
#include <stdlib.h>

Tracker::Tracker(int _edge_mode, int _edge, int _laser_pin)
  : edge_mode(_edge_mode),
    edge(_edge),
    laser_pin(_laser_pin)
{

  servo = new Servo();
  servo->attach(9);
  debug->log("Attaching servo to port 9");

  laser_timer = new Timer();
  pinMode(laser_pin, OUTPUT);
  // laser_timer->oscillate(laser_pin, 100, LOW);
  laser_timer->every(1000, this, toggle_laser_wrapper);
}

void Tracker::loop(void)
{
  laser_timer->update();
}

// void Tracker::move_servo(void)
// {
//   char out_buffer[100];
//   for(pos = 0; pos < 180; pos += 20)  // goes from 0 degrees to 180 degrees
//   {
//     itoa(pos, out_buffer, 10);                      // in steps of 1 degree
//     debug->log(out_buffer);
//     servo->write(pos);            // tell servo to go to position in variable 'pos'
//   }
//   for(pos = 180; pos>=1; pos-=20)     // goes from 180 degrees to 0 degrees
//   {
//     itoa(pos, out_buffer, 10);                      // in steps of 1 degree
//     debug->log(out_buffer);
//     servo->write(pos);          // tell servo to go to position in variable 'pos'
//   }
// }

void Tracker::toggle_laser()
{
  debug->log("Toggling Laser");
  *portInputRegister(digitalPinToPort(laser_pin)) = digitalPinToBitMask(laser_pin);
}

void Tracker::toggle_laser_wrapper(void *self)
{
  ((Tracker *)self)->toggle_laser();
}