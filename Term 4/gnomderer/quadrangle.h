#ifndef QUADRANGLE_H_
#define QUADRANGLE_H_

#include "entity.h"

class Quadrangle : public Entity {
 public:
  Quadrangle(Point a, Point b, Point c, Point d, sf::Color color);

  virtual Vector Normal(Point p);

  virtual bool Intersection(Ray ray, Point *intersection);
  
  virtual sf::Color GetColor(Point p);

 private:
  Point a_, c_;
  Vector ab_, ad_;
  Vector cb_, cd_;
  Vector normal_;
  sf::Color color_;
};

#endif // QUADRANGLE_H_
