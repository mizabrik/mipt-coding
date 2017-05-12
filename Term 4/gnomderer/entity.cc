#include "entity.h"

Entity::Entity() {
  material_.color = Color::White;
  material_.opacity = 1.0;
  material_.reflectance = 0.0;
  material_.refractive_index = 0.0;
}

sf::Color Entity::GetColor(Point p) const {
  return material_.color;
}

Box Entity::BoundingBox() const {
  return box_;
}

double Entity::GetOpacity() const {
  return material_.opacity;
}

double Entity::GetReflectance() const {
  return material_.reflectance;
}

double Entity::GetRefractiveIndex() const {
  return material_.refractive_index;
}

void Entity::SetMaterial(const Material &material) {
  material_ = material;
}
