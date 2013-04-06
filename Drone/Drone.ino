
#include "Communication.h"
#include "Debug.h"
#include "Drone.h"
#include "Metro.h"
#include "MemoryFree.h"
#include "Motor.h"
#include "Servo.h"
#include "Tracker.h"

// Tracker *tracker;
Metro *motor_timer;
Metro *free_memory_timer;
// Motor *motor;
Message_t *message;
Comm *queen;

Message_t *heartbeat_message;

void setup()
{
  // tracker = new Tracker(ONE_EDGE, LEFT_EDGE, 2, 5);
  // motor = new Motor(5, 6, 7);
  debug->log("Starting UP");
  // motor->set(255, CW);

  motor_timer = new Metro(1000);
  free_memory_timer = new Metro(100);
  heartbeat_message = new Message_t;

  queen = new Comm(2, 3);
}

void loop()
{
  if (motor_timer->check())
  {
    // motor->set(255, (MotorDirection)!motor->direction);
  }

  message = queen->loop();
  if (message != NULL)
  {
    delegate_message(message);
  }
  if (free_memory_timer->check())
  {
    // debug->log("Free Memory: %d", freeMemory());
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
    case 0:
      debug->log("Heartbeat Received");
      heartbeat_message->from = DRONE_ID;
      heartbeat_message->to = 0;
      heartbeat_message->type = MT_HEARTBEAT;
      queen->send(heartbeat_message);
      debug->log("Sent heartbeat (FM: %d)", freeMemory());
      break;
  }
  delete message;
}