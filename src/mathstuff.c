#include "mathstuff.h"

double ms_squaredDist(Point p1, Point p2) {
  const double dx = p1.x - p2.x;
  const double dy = p1.y - p2.y;
  return dx * dx + dy * dy;
}
