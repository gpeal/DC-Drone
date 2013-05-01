#ifndef MOTOR_H
#define MOTOR_H

#define MOTOR_CW 0
#define MOTOR_CCW 1

enum MotorDirection {CW, CCW};

class Motor
{
public:
  Motor(const int _speed_pin, const int _direction_pin);

  void set(const int _speed, const MotorDirection _direction);

  int speed;
  MotorDirection direction;

private:
  const int speed_pin;
  const int direction_pin;
};

#endif