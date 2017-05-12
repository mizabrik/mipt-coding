#ifndef QUADRANGLE_H_
#define QUADRANGLE_H_

#include "entity.h"

class Quadrangle : public Entity {
 public:
  Quadrangle(Point a, Point b, Point c, Point d);

  virtual Vector Normal(Point p) const;

  virtual bool Intersection(Ray ray, Point *intersection) const;

  virtual sf::Color GetColor(Point p) const;

 private:
  Point a_, c_;
  Vector ab_, ad_;
  Vector cb_, cd_;
  Vector normal_;
};

#endif // QUADRANGLE_H_
