#ifndef UTILS_H
#define UTILS_H

typedef struct Coordinate
{
  float x;
  float y;
};

// cap the number between the minimum and maximum values
int cap(int, int min, int max);
float cap(float, float min, float max);

#endif