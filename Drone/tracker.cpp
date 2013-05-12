#include <stdlib.h>
#include <math.h>
#include "Arduino.h"
#include "Tracker.h"
#include "Utils.h"
#include "StateMachine.h"

/*
 * PHOTOTRANSISTOR CIRCUIT
 * RED END -> ANALOG IN AND 47K -> GROUND
 * BLACK END -> +5V
 * The colors are reversed because the red end is actually just the emitter and the black end is just the collector
 */


Tracker::Tracker(int transistor_pin_left, int transistor_pin_middle, int transistor_pin_right)
{
  left_sensor = new Sensor(transistor_pin_left);
  middle_sensor = new Sensor(transistor_pin_middle);
  right_sensor = new Sensor(transistor_pin_right);

  left_sensor->calibrate();
  middle_sensor->calibrate();
  right_sensor->calibrate();

  init();
}

Tracker::Tracker(int transistor_pin_left, int transistor_pin_right)
{
  left_sensor = new Sensor(transistor_pin_left);
  middle_sensor = NULL;
  right_sensor = new Sensor(transistor_pin_right);

  left_sensor->calibrate();
  right_sensor->calibrate();

  init();
}

void Tracker::init(void)
{
  state = TRACKER_STATE_NONE;
  motor_driver = MotorDriver::get_instance();
  execute_timer = new Metro(20);
}

/**
 * Tracker::loop expects to be called in the arduino main loop and periodically
 *                updates each tracker and runs the logic code for the current state
 *
 */
void Tracker::loop(void)
{
  int middle_hit;
  if (execute_timer->check())
  {
    left_sensor->make_reading();
    if (middle_sensor != NULL)
      middle_sensor->make_reading();
    right_sensor->make_reading();
    Sensor::toggle_laser();

    // if the tracker was only initialized with 2 sensors, the middle will be NULL
    if (middle_sensor == NULL)
    {
      middle_hit = 0;
    }
    else
    {
      middle_hit = (int)middle_sensor->recently_hit_prey();
    }

    state = ((int)left_sensor->recently_hit_prey() << 2) | (middle_hit << 1) | ((int)right_sensor->recently_hit_prey());
  }
}