
#include "Communication.h"
#include "Debug.h"
#include "Drone.h"
#include "Metro.h"
#include "MemoryFree.h"
#include "Motor.h"
#include "Servo.h"
#include "StateMachine.h"
#include "Tracker.h"

Tracker *tracker;
Metro *motor_timer;
Metro *free_memory_timer;
// Motor *motor;
Comm *queen;

Message_t *message;

void setup()
{
  debug->log("Starting UP");
  queen = new Comm(3, 4);
  tracker = new Tracker(ONE_EDGE, LEFT_EDGE, 2, 5, 9);
  // motor = new Motor(5, 6, 7);
  // motor->set(255, CW);

  motor_timer = new Metro(1000);
  free_memory_timer = new Metro(1000);
  message = new Message_t;
}

void loop()
{
  tracker->loop();
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
    if (freeMemory() < 500)
    {
      debug->log("Free Memory: %d", freeMemory());
    }
  }
}

void send_heartbeat(void)
{
  message->type = MT_HEARTBEAT;
  sprintf(message->payload, "%d,%d", StateMachine::state(), freeMemory());
  queen->send(message);
  debug->log("Sent heartbeat");
}

/**
 * delegate_message takes a Message_t struct and takes action on it
 *
 * @param Message_t *message: the message to take action on
 */
void delegate_message(Message_t *message)
{
  char str1[16];
  char str2[16];
  int int1 = 6;
  float float1 = 6.66;
  char char1 = '7';
  char formatter[16];
  int num_matched;
  message->from = DRONE_ID;
  message-> to = QUEEN_ID;
  switch(message->type)
  {
    case MT_INITIALIZE:
      debug->log("Initialize Received (%s)", message->payload);
      sprintf(formatter, "%%[^%c]%c%%d%c%%[^%c]%c%%c", PAYLOAD_DELIMITER, PAYLOAD_DELIMITER, PAYLOAD_DELIMITER, PAYLOAD_DELIMITER, PAYLOAD_DELIMITER);
      debug->log("Formatter: %s", formatter);
      num_matched = sscanf(message->payload, formatter, str1, &int1, str2, &char1);
      float1 = atof(str2);
      debug->log("Matched: %d. %s, %d, %c, %d", num_matched, str1, int1, char1, (int)(100 * float1));
      send_heartbeat();
      break;
    case MT_HEARTBEAT:
      debug->log("Heartbeat Received");
      send_heartbeat();
      break;
  }
  delete message;
}