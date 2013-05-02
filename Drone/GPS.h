#ifndef GPS_H
#define GPS_H
#include <QueueList.h>
#include "Motor.h"
#include "Odometry.h"
#include "Utils.h"

// the max speed (0-1)
#define SPEED 1
#define target targets.peek()

class GPS
{
public:
  GPS(Motor *left_motor, Motor *right_motor, Odometry *odometry);
  void loop(void);
  void move_to(float x, float y);
  bool reached_target(void);
  void add_waypoint(Coordinate waypoint);
private:
  float heading_to_target(void);
  double distance_from_target(void);
  Motor *left_motor;
  Motor *right_motor;
  Odometry *odometry;
  QueueList <Coordinate> targets;
  Metro *timer;
};

#endif