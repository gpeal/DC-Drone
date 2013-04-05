#include <Servo.h>

#include "Communication.h"
#include "Debug.h"
#include "Metro.h"
#include "Motor.h"
#include "Tracker.h"

Tracker *tracker;
Metro *motor_timer;
Motor *motor;
Message_t *message;

void setup()
{
  // tracker = new Tracker(ONE_EDGE, LEFT_EDGE, 2, 5);
  motor = new Motor(5, 6, 7);
  debug->log("Starting UP");
  motor->set(255, CW);

  motor_timer = new Metro(1000);

  Comm->setup();
}

void loop()
{
  if (motor_timer->check())
  {
    motor->set(255, (MotorDirection)!motor->direction);
  }
  message = Comm->loop();
  if (message != NULL)
  {
    delegate_message(message);
  }
}

/**
 * delegate_message takes a Message_t struct and takes action on it
 *
 * @param Message_t *message: the message to take action on
 */
void delegate_message(Message_t *message)
{
  switch(message->type)
  {

  }
}