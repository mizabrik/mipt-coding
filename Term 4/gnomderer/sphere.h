#ifndef SPHERE_H_
#define SPHERE_H_

#include "entity.h"

class Sphere : public Entity {
 public:
  Sphere(Point center, Real r);

  virtual Vector Normal(Point p) const;

  virtual bool Intersection(Ray ray, Point *intersection) const;

 private:
  Point center_;
  Real r_;
};

#endif // SPHERE_H_
