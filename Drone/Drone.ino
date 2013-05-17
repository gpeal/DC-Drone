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

int DRONE_ID;

Metro *free_memory_timer;
MotorDriver *motor_driver;

void setup()
{
  DRONE_ID = EEPROM.read(DRONE_ID_EEPROM);
  debug->log("Starting UP Drone %d", DRONE_ID);

  // initial state
  StateMachine::enter(StateMachine::ATTACKING);

  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, OUTPUT);
  Sensor::set_laser_pin(19);

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
  motor_driver = MotorDriver::get_instance();

  Tracker *tracker = new Tracker(0, 1, 4);

  StateMachine::Deploying::motor_driver = motor_driver;

  StateMachine::Relocating::motor_driver = motor_driver;

  StateMachine::Attacking::motor_driver = motor_driver;
  StateMachine::Attacking::tracker = tracker;
  Sonar::prey_sonar = new NewPing(5, 4, 150);
  // use the prey lasers until we get the top lasers
  StateMachine::Returning::tracker = new Tracker(0, 4);
  StateMachine::Returning::motor_driver = motor_driver;
}

void loop()
{
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
  debug->log("Spinning7: %d", (int)motor_driver->spinning);
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
