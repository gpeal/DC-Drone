#include <EEPROM.h>
#include <Encoder.h>
#include <Metro.h>
#include <MemoryFree.h>
#include <NewPing.h>
#include <Servo.h>
#include <QueueList.h>
#include "Attacking.h"
#include "Capturing.h"
#include "Communication.h"
#include "Debug.h"
#include "Drone.h"
#include "Motor.h"
#include "MotorDriver.h"
#include "Returning.h"
#include "Searching.h"
#include "Sensor.h"
#include "Sonar.h"
#include "StateMachine.h"
#include "Tracker.h"
#include "Utils.h"

int DRONE_ID;

Metro *free_memory_timer;
Comm *queen;
MotorDriver *motor_driver;
// initialize static vars
Motor *MotorDriver::left_motor;
Motor *MotorDriver::right_motor;
MotorDriver *MotorDriver::instance;


Message_t *message;
// initialize the static int Sensor::laser_pin
int Sensor::laser_pin;

void setup()
{
  DRONE_ID = EEPROM.read(DRONE_ID_EEPROM);
  debug->log("Starting UP Drone %d", DRONE_ID);

  // initial state
  StateMachine::enter(StateMachine::SEARCHING);

  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, OUTPUT);
  Sensor::set_laser_pin(19);

  MotorDriver::left_motor = new Motor(3, 12);
  MotorDriver::right_motor = new Motor(11, 13);
  motor_driver = MotorDriver::get_instance();

  // queen = new Comm(6, 7);

  set_state_objects();

  free_memory_timer = new Metro(10000);
  // debug->log("Battery Voltage: %d", (int)readVcc());
}

/**
 * give the state namespaces access to any objects that they will need
 */
void set_state_objects(void)
{
  Tracker *tracker = new Tracker(0, 1, 4);
  StateMachine::Searching::motor_driver = motor_driver;
  StateMachine::Searching::tracker = tracker;
  StateMachine::Attacking::motor_driver = motor_driver;
  StateMachine::Attacking::tracker = tracker;
  Sonar::prey_sonar = new NewPing(5, 4, 200);
  // use the prey lasers until we get the top lasers
  // StateMachine::Returning::tracker = new Tracker(0, 1);
  StateMachine::Returning::motor_driver = motor_driver;
}

void loop()
{
  // message = queen->loop();
  // if (message != NULL)
  // {
  //   delegate_message(message);
  // }

  Sonar::loop();

  switch(StateMachine::state())
  {
    case StateMachine::SEARCHING:
      StateMachine::Searching::loop();
      break;
    case StateMachine::ATTACKING:
      StateMachine::Attacking::loop();
      break;
    case StateMachine::RETURNING:
      StateMachine::Returning::loop();
      return;
    case StateMachine::CAPTURING:
      StateMachine::Capturing::loop();
      break;
  }

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
  debug->log("Sent heartbeat");
  queen->send(message);
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

void attack(void)
{
}