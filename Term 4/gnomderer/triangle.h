#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include <SFML/Graphics.hpp>
#include "entity.h"

class Triangle : public Entity {
 public:
  Triangle(Point a, Point b, Point c);

  virtual Vector Normal(Point p) const;

  virtual bool Intersection(Ray ray, Point *intersection) const;

  void Barycentric(Point p, Real *u, Real *v) const;

 private:
  Point a_;
  Vector ab_, ac_;
  Vector normal_;
};

#endif // TRIANGLE_H_
