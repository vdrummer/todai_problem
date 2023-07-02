#ifndef TODAIGRAPH_MATHSTUFF_H
#define TODAIGRAPH_MATHSTUFF_H

typedef struct point Point;
struct point {
  double x;
  double y;
};

double ms_squaredDist(Point p1, Point p2);

#endif // TODAIGRAPH_MATHSTUFF_H
