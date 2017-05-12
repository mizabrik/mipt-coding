#include "triangle.h"

#include <cmath>

#include "geometry.h"

Triangle::Triangle(Point a, Point b, Point c)
    : a_(a),
      ab_(b - a),
      ac_(c - a),
      normal_(Normed(Cross(ab_, ac_))) {
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

Vector Triangle::Normal(Point p) const {
  return normal_;
}

bool Triangle::Intersection(Ray ray, Point *intersection) const {
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

void Triangle::Barycentric(Point p, Real *u, Real *v) const {
  Real d00 = Dot(ab_, ab_);
  Real d01 = Dot(ab_, ac_);
  Real d11 = Dot(ac_, ac_);
  Real d20 = Dot(p - a_, ab_);
  Real d21 = Dot(p - a_, ac_);
  Real inv_det = 1 / (d00 * d11 - d01 * d01);
  *v = (d11 * d20 - d01 * d21) * inv_det;
  *u = (d00 * d21 - d01 * d20) * inv_det;
}

sf::Color Triangle::GetColor(Point point) const {
  auto &texture = material_.texture;
  if (texture.getSize().x == 0 || texture.getSize().y == 0)
    return material_.color;

  Ray ray{point + -1 * normal_, normal_};
  Vector p = Cross(ray.direction(), ac_);
  Real det = Dot(ab_, p);

  if (det == 0)
      return Color::Black;
  Real inv_det = 1 / det;

  Vector oa = ray.origin() - a_;

  Real u = Dot(oa, p) * inv_det;
  if (u < 0 || u > 1)
    return Color::Black;

  Vector q = Cross(oa, ab_);
  Real v = Dot(ray.direction(), q) * inv_det;
  if (v < 0 || u + v > 1)
    return Color::Black;

  Real distance = Dot(ac_, q) * inv_det;
  if (distance <= 0) {
    return Color::Black;
  }

  int x = std::round((u * texture.getSize().x).value());
  int y = std::round(((1 - u - v) * texture.getSize().y).value());

  return texture.getPixel(x, y);
}
