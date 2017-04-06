#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include "entity.h"

class Triangle : public Entity {
 public:
  Triangle(Point a, Point b, Point c, sf::Color color);

  virtual Vector Normal(Point p);

  virtual bool Intersection(Ray ray, Point *intersection);
  
  virtual sf::Color GetColor(Point p);

 private:
  Point a_;
  Vector ab_, ac_;
  Vector normal_;
  sf::Color color_;
};

#endif // TRIANGLE_H_
