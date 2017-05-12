#include "quadrangle.h"

#include <cmath>

#include "geometry.h"

Quadrangle::Quadrangle(Point a, Point b, Point c, Point d)
    : a_(a),
      c_(c),
      ab_(b - a),
      ad_(d - a),
      ac_(c - a),
      cb_(b - c),
      cd_(d - c),
      normal_(Normed(Cross(ab_, ad_))) {
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

Vector Quadrangle::Normal(Point p) const {
  return normal_;
}

bool Quadrangle::Intersection(Ray ray, Point *intersection) const {
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

sf::Color Quadrangle::GetColor(Point point) const {
  auto &texture = material_.texture;
  if (texture.getSize().x == 0 || texture.getSize().y == 0)
    return material_.color;

  Ray ray{point + (-1)*normal_, normal_};
  Vector p = Cross(ray.direction(), ad_);
  Real det = Dot(ab_, p);
  if (det == 0)
    return Color::Black;
  Real inv_det = 1 / det;

  Vector ao = ray.origin() - a_;
  Real alpha = Dot(ao, p) * inv_det;
  if (alpha < 0)
    return Color::Black;

  Vector q = Cross(ao, ab_);
  Real beta = Dot(ray.direction(), q) * inv_det;
  if (beta < 0)
    return Color::Black;

  if (alpha + beta > 1) {
    Vector p2 = Cross(ray.direction(), cb_);
    Real det2 = Dot(cd_, p2);
    if (det2 == 0)
      return Color::Black;
    Real inv_det2 = 1 / det2;

    Vector co = ray.origin() - c_;
    if (Dot(co, p2) * inv_det2 < 0)
      return Color::Black;
    Vector q2 = Cross(co, cd_);
    if (Dot(ray.direction(), q2) * inv_det2 < 0)
      return Color::Black;
  }

  Real distance = Dot(ad_, q) * inv_det;
  if (distance <= 0) {
    return Color::Black;
  }

  Real alpha1, beta1;
  Real u, v;
  auto n = Cross(ab_, ad_);
  if (n.x.abs() >= n.y.abs()
      && n.x.abs() >= n.z.abs()) {
    alpha1 = (ac_.y * ad_.z - ac_.z * ad_.y) / n.x;
    beta1 = (ab_.y * ac_.z - ab_.z * ac_.y) / n.x;
  } else if (n.y.abs() >= n.x.abs()
             && n.y.abs() >= n.z.abs()) {
    alpha1 = (ac_.z * ad_.x - ac_.x * ad_.z) / n.y;
    beta1 = (ab_.z * ac_.x - ab_.x * ac_.z) / n.y;
  } else {
    alpha1 = (ac_.x * ad_.y - ac_.y * ad_.x) / n.z;
    beta1 = (ab_.x * ac_.y - ab_.y * ac_.x) / n.z;
  }


  if (alpha1 == 1) {
    u = alpha;
    if (beta1 == 1) {
      v = beta;
    } else {
      v = beta / (u * (beta1 - 1) + 1);
    }
  } else if (beta1 == 1) {
    v = beta;
    u = alpha / (v * (alpha1 - 1) + 1);
  } else {
    Real a = 1 -beta1;
    Real b = alpha * (beta1 - 1) - beta * (alpha1 - 1) - 1;
    Real c = alpha;
    Real sol = -(b + b.sign() * (b * b - 4 * a * c).Sqrt()) / 2;
    u = sol / a;
    if (u < 0 || u > 1) {
      u = c / sol;
    }
    v = beta / (u * (beta1 - 1) + 1);
  }

  int x = std::round((u * texture.getSize().x).value());
  int y = std::round(((1 - v) * texture.getSize().y).value());
  return texture.getPixel(x, y);
}
