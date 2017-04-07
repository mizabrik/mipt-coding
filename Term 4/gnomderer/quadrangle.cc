#include "quadrangle.h"

#include "geometry.h"

Quadrangle::Quadrangle(Point a, Point b, Point c, Point d, sf::Color color)
    : a_(a),
      c_(c),
      ab_(b - a),
      ad_(d - a),
      cb_(b - c),
      cd_(d - c),
      normal_(Normed(Cross(ab_, ad_))),
      color_(color) {
  box_ = Box{
    {
      std::min(std::min(a.x, b.x), std::min(c.x, d.x)),
      std::min(std::min(a.y, b.y), std::min(c.y, d.y)),
      std::min(std::min(a.z, b.z), std::min(c.z, d.z))
    },
    {
      std::max(std::max(a.x, b.x), std::max(c.x, d.x)),
      std::max(std::max(a.y, b.y), std::max(c.y, d.y)),
      std::max(std::max(a.z, b.z), std::max(c.z, d.z))
    }
  };
}

Vector Quadrangle::Normal(Point p) {
  return normal_;
}

bool Quadrangle::Intersection(Ray ray, Point *intersection) {
  // http://graphics.cs.kuleuven.be/publications/LD05ERQIT/LD05ERQIT_paper.pdf
  Vector p = Cross(ray.direction(), ad_);
  Real det = Dot(ab_, p);
  if (det == 0)
    return false;
  Real inv_det = 1 / det;

  Vector ao = ray.origin() - a_;
  Real alpha = Dot(ao, p) * inv_det;
  if (alpha < 0)
    return false;

  Vector q = Cross(ao, ab_);
  Real beta = Dot(ray.direction(), q) * inv_det;
  if (beta < 0)
    return false;

  if (alpha + beta > 1) {
    Vector p2 = Cross(ray.direction(), cb_);
    Real det2 = Dot(cd_, p2);
    if (det2 == 0)
      return false;
    Real inv_det2 = 1 / det2;

    Vector co = ray.origin() - c_;
    if (Dot(co, p2) * inv_det2 < 0)
      return false;
    Vector q2 = Cross(co, cd_);
    if (Dot(ray.direction(), q2) * inv_det2 < 0)
      return false;
  }

  Real distance = Dot(ad_, q) * inv_det;
  if (distance > 0) {
    *intersection = ray.GetPoint(distance);
    return true;
  } else {
    return false;
  }
}

sf::Color Quadrangle::GetColor(Point p) {
  return color_;
};
