#include "sphere.h"

#include "geometry.h"

Sphere::Sphere(Point center, Real r, sf::Color color)
    : center_(center), r_(r), color_(color) {}

Vector Sphere::Normal(Point p) {
  return Normed(p - center_);
}

bool Sphere::Intersection(Ray ray, Point *intersection) {
  // t^2 + 2(P - 0, d) t + |P - O|^2 - R^2 = 0
  Real hf_b = Dot(ray.origin() - center_, ray.direction());
  Real c = Length2(ray.origin() - center_) - r_ * r_;

  Real qr_d = hf_b * hf_b - c;
  if (qr_d <= 0) {
    return false;
  }

  Real base = -hf_b;
  Real delta = qr_d.Sqrt();
  if (base + delta <= 0) {
    return false;
  } else if (base - delta > 0) {
    *intersection = ray.GetPoint(base - delta);
  } else {
    *intersection = ray.GetPoint(base + delta);
  }

  return true;
};

sf::Color Sphere::GetColor(Point p) {
  return color_;
};
