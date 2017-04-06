#ifndef SPHERE_H_
#define SPHERE_H_

#include "entity.h"

class Sphere : public Entity {
 public:
  Sphere(Point center, Real r, sf::Color color);

  virtual Vector Normal(Point p);

  virtual bool Intersection(Ray ray, Point *intersection);
  
  virtual sf::Color GetColor(Point p);

 private:
  Point center_;
  Real r_;
  sf::Color color_;
};

#endif // SPHERE_H_
