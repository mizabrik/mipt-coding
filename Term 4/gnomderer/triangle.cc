#include "triangle.h"

#include "geometry.h"

Triangle::Triangle(Point a, Point b, Point c, sf::Color color)
    : a_(a),
      ab_(b - a),
      ac_(c - a),
      normal_(Normed(Cross(ab_, ac_))),
      color_(color) {
  box_ = Box{
    {
      std::min(a.x, std::min(b.x, c.x)),
      std::min(a.y, std::min(b.y, c.y)),
      std::min(a.z, std::min(b.z, c.z))
    },
    {
      std::max(a.x, std::max(b.x, c.x)),
      std::max(a.y, std::max(b.y, c.y)),
      std::max(a.z, std::max(b.z, c.z))
    }
  };
}

Vector Triangle::Normal(Point p) {
  return normal_;
}

bool Triangle::Intersection(Ray ray, Point *intersection) {
  Vector p = Cross(ray.direction(), ac_);
  Real det = Dot(ab_, p);

  if (det == 0)
      return false;
  Real inv_det = 1 / det;

  Vector oa = ray.origin() - a_;

  Real u = Dot(oa, p) * inv_det;
  if (u < 0 || u > 1)
    return false;

  Vector q = Cross(oa, ab_);
  Real v = Dot(ray.direction(), q) * inv_det;
  if (v < 0 || u + v > 1)
    return false;

  Real distance = Dot(ac_, q) * inv_det;
  if (distance > 0) {
    *intersection = ray.GetPoint(distance);
    return true;
  } else {
    return false;
  }
};

sf::Color Triangle::GetColor(Point p) {
  return color_;
};
