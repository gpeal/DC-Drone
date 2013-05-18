#include <EEPROM.h>
#include <Encoder.h>
#include <Metro.h>
#include <MemoryFree.h>
#include <NewPing.h>
#include <Servo.h>
#include <QueueList.h>
#include <XBee.h>
#include "Attacking.h"
#include "Debug.h"
#include "Deploying.h"
#include "Drone.h"
#include "Motor.h"
#include "MotorDriver.h"
#include "Relocating.h"
#include "Returning.h"
#include "Sensor.h"
#include "Sonar.h"
#include "StateMachine.h"
#include "Tracker.h"
#include "Utils.h"
/*
 * Drone is our robot for the Northwestern Design Competition
 */

int DRONE_ID;

Metro *free_memory_timer;
MotorDriver *motor_driver;

void setup()
{
  DRONE_ID = EEPROM.read(DRONE_ID_EEPROM);
  debug->log("Starting UP Drone %d", DRONE_ID);
  set_state_objects();

  // initial state
  StateMachine::enter(StateMachine::DEPLOYING);

  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, OUTPUT);
  Sensor::set_laser_pin(9);
  Sensor::laser_state = HIGH;


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
  motor_driver = MotorDriver::get_instance();

  Tracker *tracker = new Tracker(0, 1, 2);

  StateMachine::Deploying::motor_driver = motor_driver;

  StateMachine::Relocating::motor_driver = motor_driver;

  StateMachine::Attacking::motor_driver = motor_driver;
  StateMachine::Attacking::tracker = tracker;
  Servo *left_servo = new Servo();
  left_servo->attach(6);
  left_servo->write(0);
  Servo *right_servo = new Servo();
  right_servo->attach(5);
  right_servo->write(0);
  StateMachine::Attacking::left_servo = left_servo;
  StateMachine::Attacking::right_servo = right_servo;


  Sonar::prey_sonar = new NewPing(7, 8, 500);
  Sonar::nest_sonar = new NewPing(2, 4, 500);
  // use the prey lasers until we get the top lasers
  StateMachine::Returning::tracker = new Tracker(3, 4);
  StateMachine::Returning::motor_driver = motor_driver;
}

void loop()
{
  // StateMachine::Attacking::tracker->loop();
  // StateMachine::Attacking::motor_driver->set(255, 255);
  // delay(2000);
  // StateMachine::Attacking::motor_driver->set(-255, -255);
  // return;


  motor_driver->loop();
  switch(StateMachine::state())
  {
    case StateMachine::DEPLOYING:
      StateMachine::Deploying::loop();
      break;
    case StateMachine::RELOCATING:
      StateMachine::Relocating::loop();
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
