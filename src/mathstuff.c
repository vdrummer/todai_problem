#include "mathstuff.h"
#include <math.h>

double ms_squaredDist(Point p1, Point p2) {
  const double dx = p1.x - p2.x;
  const double dy = p1.y - p2.y;
  return dx * dx + dy * dy;
}

double ms_isLeft(Point p0, Point p1, Point p2) {
  return (p1.x - p0.x) * (p2.y - p0.y) -
         (p2.x - p0.x) * (p1.y - p0.y);
}

bool ms_pointInRectangle(Point p, Point x, Point y, Point z, Point w) {
  return ms_isLeft(x, y, p) > 0 &&
    ms_isLeft(y, z, p) > 0 &&
    ms_isLeft(z, w, p) > 0 &&
    ms_isLeft(w, x, p) > 0;
}

Point ms_normal(Point p) {
  const double length = sqrt(p.x * p.x + p.y * p.y);
  p.x /= length;
  p.y /= length;

  return p;
}

Point ms_unitNormalVector(Point p1, Point p2) {
  Point result = {0};

  const double dx = p2.x - p1.x;
  const double dy = p2.y - p1.y;
  result.x = -1 * dy;
  result.y = dx;

  return ms_normal(result);
}
