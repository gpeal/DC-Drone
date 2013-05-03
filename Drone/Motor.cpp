#include "Arduino.h"
#include "Motor.h"

Motor::Motor(const int speed_pin, const int direction_pin)
  : speed_pin(speed_pin),
    direction_pin(direction_pin)
{
  pinMode(speed_pin, OUTPUT);
  pinMode(direction_pin, OUTPUT);
  speed = 0;
}

void Motor::set(const int _speed, const MotorDirection _direction)
{
  speed = _speed;
  direction = _direction;
  digitalWrite(direction_pin, direction);
  analogWrite(speed_pin, speed);
}