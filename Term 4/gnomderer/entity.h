#ifndef ENTITY_H_
#define ENTITY_H_

#include <SFML/Graphics/Color.hpp>

#include "geometry.h"

class Entity {
 public:
  virtual Vector Normal(Point p) = 0;

  virtual bool Intersection(Ray r, Point *intersection) = 0;
  
  virtual sf::Color GetColor(Point p) = 0;

  virtual Box BoundingBox() const;

  virtual ~Entity() {};

  double alpha = 0;
  double refraction = 0;

 protected:
  Box box_;
};

#endif // ENTITY_H_
