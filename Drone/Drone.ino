#include <Servo.h>

#include "Debug.h"
#include "Metro.h"
#include "Motor.h"
#include "Tracker.h"

Tracker *tracker;
Metro *motor_timer;
Motor *motor;

void setup()
{
  // tracker = new Tracker(ONE_EDGE, LEFT_EDGE, 2, 5);
  motor = new Motor(2, 3, 4);
  debug->log("Starting UP");
  motor->set(255, CW);

  motor_timer = new Metro(1000);
}

void loop()
{
  if (motor_timer->check())
  {
    motor->set(255, (MotorDirection)!motor->direction);
  }
  // tracker->loop();
}