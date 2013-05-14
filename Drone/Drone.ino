#include <EEPROM.h>
#include <Encoder.h>
#include <Metro.h>
#include <MemoryFree.h>
#include <NewPing.h>
#include <Servo.h>
#include <SoftwareSerial.h>
#include <QueueList.h>
#include <XBee.h>
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


Message_t *message;
// initialize the static int Sensor::laser_pin
int Sensor::laser_pin;

void setup()
{
  DRONE_ID = EEPROM.read(DRONE_ID_EEPROM);
  debug->log("Starting UP Drone %d", DRONE_ID);

  // initial state
  StateMachine::enter(StateMachine::IDLE);

  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, OUTPUT);
  Sensor::set_laser_pin(19);


  queen = new Comm(6, 7);

  set_state_objects();

  free_memory_timer = new Metro(10000);
  // debug->log("Battery Voltage: %d", (int)readVcc());
}

/**
 * give the state namespaces access to any objects that they will need
 */
void set_state_objects(void)
{
  MotorDriver::left_motor = new Motor(3, 12);
  MotorDriver::right_motor = new Motor(11, 13);
  MotorDriver *motor_driver = MotorDriver::get_instance();
  Tracker *tracker = new Tracker(0, 1, 4);
  StateMachine::Searching::motor_driver = motor_driver;
  StateMachine::Searching::tracker = tracker;
  StateMachine::Attacking::motor_driver = motor_driver;
  StateMachine::Attacking::tracker = tracker;
  Sonar::prey_sonar = new NewPing(5, 4, 200);
  // use the prey lasers until we get the top lasers
  // StateMachine::Returning::tracker = new Tracker(2, 3);
  StateMachine::Returning::motor_driver = motor_driver;
}

void loop()
{
  message = queen->loop();
  if (message != NULL)
  {
    delegate_message(message);
  }

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
      queen->send(MT_INITIALIZE_RESPONSE, "");
      break;
    case MT_HEARTBEAT:
      debug->log("Heartbeat Received");
      send_heartbeat();
      break;
  }
  delete message;
}

void send_heartbeat(void)
{
  int type;
  char msg[MAX_PAYLOAD_LENGTH];
  switch(StateMachine::state())
  {
    case StateMachine::IDLE:
      type = MT_HEARTBEAT_RESPONSE_IDLE;
      sprintf(msg, "%d", (int)(millis() - StateMachine::enter_millis));
      break;
    case StateMachine::DEPLOYING:
      type = MT_HEARTBEAT_RESPONSE_DEPLOYING;
      // vars: duration, complete
      sprintf(msg, "");
      break;
    case StateMachine::SEARCHING:
      type = MT_HEARTBEAT_RESPONSE_SEARCHING;
      // vars: duration, tracking
      sprintf(msg, "");
      break;
    case StateMachine::RELOCATING:
      type = MT_HEARTBEAT_RESPONSE_RELOCATING;
      // vars:duration,
      sprintf(msg, "");
      break;
    case StateMachine::ATTACKING:
      type = MT_HEARTBEAT_RESPONSE_ATTACKING;
      // duration, distance, tracker state, captured
      sprintf(msg, "");
      break;
    case StateMachine::SEARCHING_NEST:
      type = MT_HEARTBEAT_RESPONSE_SEARCHING_NEST;
      sprintf(msg, "");
      // duration, tracking
      break;
    case StateMachine::RETURNING:
      type = MT_HEARTBEAT_RESPONSE_RETURNING;
      sprintf(msg, "");
      // duration, tracking, complete
      break;
    case StateMachine::DELIVERING:
      type = MT_HEARTBEAT_RESPONSE_DELIVERING;
      sprintf(msg, "");
      break;
    case StateMachine::DISCONNECTED:
      type = MT_HEARTBEAT_RESPONSE_DISCONNECTED;
      // duration
      sprintf(msg, "");
      break;
  }
  queen->send(type, msg);
  debug->log("Sent heartbeat");
}