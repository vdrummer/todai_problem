#ifndef TODAIGRAPH_MATHSTUFF_H
#define TODAIGRAPH_MATHSTUFF_H

#include <stdbool.h>

typedef struct point Point;
struct point {
  double x;
  double y;
};

double ms_squaredDist(Point p1, Point p2);
double ms_isLeft(Point p0, Point p1, Point p2);
bool ms_pointInRectangle(Point p, Point x, Point y, Point z, Point w);
Point ms_normal(Point p);
Point ms_unitNormalVector(Point p1, Point p2);

#endif // TODAIGRAPH_MATHSTUFF_H
