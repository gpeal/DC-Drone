#include <Encoder.h>
#include <Metro.h>
#include <MemoryFree.h>
#include <Servo.h>
#include <QueueList.h>
#include "Communication.h"
#include "Debug.h"
#include "Drone.h"
#include "Motor.h"
#include "MotorDriver.h"
#include "Odometry.h"
#include "Sensor.h"
#include "StateMachine.h"
#include "Tracker.h"

#define PREY_CENTER_ANGLE 75

Tracker *tracker;
Metro *free_memory_timer;
Motor *left_motor;
Motor *right_motor;
Comm *queen;
Odometry *odometry;
MotorDriver *motor_driver;
// initialize static vars
Motor *MotorDriver::left_motor;
Motor *MotorDriver::right_motor;
Odometry *MotorDriver::odometry;
MotorDriver *MotorDriver::instance;
Metro search_timer(10, 1);
int search_count = 0;
Metro test_timer(2000);
int test_count = 0;


Message_t *message;
// initialize the static int Sensor::laser_pin
int Sensor::laser_pin = -1;


void setup()
{
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  left_motor = new Motor(11, 13);
  right_motor = new Motor(3, 12);
  odometry = new Odometry(Encoder(6, 7), Encoder(9, 10));
  MotorDriver::left_motor = left_motor;
  MotorDriver::right_motor = right_motor;
  MotorDriver::odometry = odometry;
  motor_driver = MotorDriver::get_instance();
  debug->log("Starting UP Drone %d", DRONE_ID);
  // queen = new Comm(2, 3);
  Sensor::set_laser_pin(2);
  tracker = new Tracker(0, 1, 4);


  free_memory_timer = new Metro(10000);
  // TODO: is it necessary to instantiate a message_t here?
  message = new Message_t;
  // output battery level
  debug->log("Battery Voltage: %d", (int)readVcc());
  motor_driver->set(0, 1);
}

void loop()
{
  long left_encoder_value, right_encoder_value;
  tracker->loop();
  odometry->loop();
  return;
  switch(StateMachine::state())
  {
    case StateMachine::SEARCHING:
      search();
      break;
    case StateMachine::ATTACKING:
      attack();
      break;
  }


  // message = queen->loop();
  // if (message != NULL)
  // {
  //   delegate_message(message);
  // }
  free_memory_check();
}

void free_memory_check(void)
{
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

void search(void)
{
  int min_ticks = 4;
  int max_ticks = 30;
  int ramp_up_time = 10;
  int ticks_to_skip =  (float)(millis() - StateMachine::enter_millis) / 1000.0 * -(float)(max_ticks - min_ticks) / (float)ramp_up_time + max_ticks;
  ticks_to_skip = cap(ticks_to_skip, min_ticks, max_ticks);

  if (search_timer.check())
  {
    search_count++;
    if (search_count % ticks_to_skip == 0)
    {
      left_motor->set(255, CW);
      right_motor->set(255, CCW);
    }
    else
    {
      left_motor->set(0, CW);
      right_motor->set(0, CW);
    }
  }
}

void attack(void)
{
  motor_driver->loop();
}