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
Comm *queen;

void setup()
{
  // tracker = new Tracker(ONE_EDGE, LEFT_EDGE, 2, 5);
  motor = new Motor(5, 6, 7);
  debug->log("Starting UP");
  motor->set(255, CW);

  motor_timer = new Metro(1000);

  queen = new Comm(2, 3);
}

void loop()
{
  if (motor_timer->check())
  {
    motor->set(255, (MotorDirection)!motor->direction);
  }
  message = queen->loop();
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