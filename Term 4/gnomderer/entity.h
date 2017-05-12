#ifndef ENTITY_H_
#define ENTITY_H_

#include <SFML/Graphics/Image.hpp>

#include "color.h"
#include "geometry.h"

class Entity {
 public:
  struct Material {
    Color color;
    double opacity;
    double reflectance;
    double refractive_index;
    sf::Image texture;
  };

  Entity();

  virtual Vector Normal(Point p) const = 0;

  virtual bool Intersection(Ray r, Point *intersection) const = 0;
  
  virtual sf::Color GetColor(Point p) const;

  virtual Box BoundingBox() const;

  double GetOpacity() const;

  double GetReflectance() const;

  double GetRefractiveIndex() const;

  void SetMaterial(const Material &material);

  virtual ~Entity() {};

 protected:
  Material material_;
  Box box_;
};

#endif // ENTITY_H_
