#include "Arduino.h"
#include "Motor.h"

Motor::Motor(const int _speed_pin, const int _direction_pin1, const int _direction_pin2)
  : speed_pin(_speed_pin),
    direction_pin1(_direction_pin1),
    direction_pin2(_direction_pin2)
{
  pinMode(speed_pin, OUTPUT);
  pinMode(direction_pin1, OUTPUT);
  pinMode(direction_pin2, OUTPUT);
  speed = 0;
}

void Motor::set(const int _speed, const MotorDirection _direction)
{
  speed = _speed;
  direction = _direction;
  switch(_direction)
  {
    case CW:
      digitalWrite(direction_pin1, 1);
      digitalWrite(direction_pin2, 0);
      break;
    case CCW:
      digitalWrite(direction_pin1, 0);
      digitalWrite(direction_pin2, 1);
  }
  analogWrite(speed_pin, speed);
}