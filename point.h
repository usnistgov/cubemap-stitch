
#ifndef POINT_H_
#define POINT_H_

#include "byte.h"

typedef struct {
  int i; 
  int j; 
  int n; 
  byte r; 
  byte g; 
  byte b;
} Pixel;

typedef struct {
  double x; 
  double y;
} Point2D;

typedef struct {
  double x;
  double y;
  double z;
} Point3D;

#endif
