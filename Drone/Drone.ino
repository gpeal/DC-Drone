#include <Encoder.h>
#include <Metro.h>
#include <MemoryFree.h>
#include <Servo.h>
#include "Communication.h"
#include "Debug.h"
#include "Drone.h"
#include "Motor.h"
#include "Odometry.h"
#include "Sensor.h"
#include "StateMachine.h"
#include "Tracker.h"

Tracker *tracker;
Metro *motor_timer;
Metro *encoder_timer;
Metro *free_memory_timer;
// Motor *motor;
Comm *queen;
Odometry *odometry;

Message_t *message;
// initialize the static int Sensor::laser_pin
int Sensor::laser_pin = -1;

void setup()
{
  debug->log("Starting UP Drone %d", DRONE_ID);
  // queen = new Comm(2, 3);
  Sensor::set_laser_pin(4);
  // tracker = new Tracker(0, 15, 1, 16);
  // motor = new Motor(5, 6, 7);
  // motor->set(255, CW);
  odometry = new Odometry(Encoder(10, 11), Encoder(12, 13));

  encoder_timer = new Metro(100);
  motor_timer = new Metro(1000);
  free_memory_timer = new Metro(1000);
  // TODO: is it necessary to instantiate a message_t here?
  message = new Message_t;
}

void loop()
{
  long left_encoder_value, right_encoder_value;
  // tracker->loop();
  if (motor_timer->check())
  {
    // motor->set(255, (MotorDirection)!motor->direction);
  }
  odometry->loop();


  // message = queen->loop();
  // if (message != NULL)
  // {
  //   delegate_message(message);
  // }
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