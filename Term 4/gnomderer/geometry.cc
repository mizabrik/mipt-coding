#include "geometry.h"

#include "real.h"

Ray::Ray(Point origin, Vector direction)
  : origin_(origin), direction_(direction / Length(direction)) {}

Ray::Ray(Point origin, Point p) : Ray(origin, p - origin) {}

Point Ray::origin() const {
  return origin_;
}

Vector Ray::direction() const {
  return direction_;
}

Point Ray::GetPoint(const Real &distance) const {
  return origin() + distance * direction();
}

Ray Ray::Mirror(const Point &p, const Vector &n) const {
  return Ray(p, direction() - 2 * Dot(direction(), n) * n);
}

std::array<Real, 3> Box::Dimensions() const {
  std::array<Real, 3> result;
  
  for (unsigned int i = 0; i < 3; ++i) {
    result[i] = max[i] - min[i];
  }

  return result;
}

Box BoxUnion(const Box &a, const Box &b) {
  Box result;

  for (unsigned int i = 0; i < 3; ++i) {
    result.min[i] = std::min(a.min[i], b.min[i]);
    result.max[i] = std::max(a.max[i], b.max[i]);
  }
  
  return result;
}

Box Box::Before(SimplePlane split) const {
  Box result = *this;
  result.max[split.axis] = split.position;
  return result;
}

Box Box::After(SimplePlane split) const {
  Box result = *this;
  result.min[split.axis] = split.position;
  return result;
}

SimplePlane Box::LeftPlane(unsigned int axis) const {
  return SimplePlane{axis, min[axis]};
}
SimplePlane Box::RightPlane(unsigned int axis) const {
  return SimplePlane{axis, max[axis]};
}

bool Box::Intersection(const Ray &ray, Real *t_in, Real *t_out) const {
  Real tx1 = LeftPlane(0).Intersection(ray);
  Real tx2 = RightPlane(0).Intersection(ray);

  Real tmin = std::min(tx1, tx2);
  Real tmax = std::max(tx1, tx2);

  Real ty1 = LeftPlane(1).Intersection(ray);
  Real ty2 = RightPlane(1).Intersection(ray);

  tmin = std::max(tmin, std::min(ty1, ty2));
  tmax = std::min(tmax, std::max(ty1, ty2));

  Real tz1 = LeftPlane(2).Intersection(ray);
  Real tz2 = RightPlane(2).Intersection(ray);

  tmin = std::max(tmin, std::min(tz1, tz2));
  tmax = std::min(tmax, std::max(tz1, tz2));

  if (tmax > tmin) {
    *t_in = tmin;
    *t_out = tmax;
    return true;
  } else {
    return false;
  }
}
  
Real SimplePlane::Intersection(Ray ray) {
  Real origin, direction;

  switch (axis) {
    case 0:
      origin = ray.origin().x;
      direction = ray.direction().x;
      break;
    case 1:
      origin = ray.origin().y;
      direction = ray.direction().y;
      break;
    case 2:
      origin = ray.origin().z;
      direction = ray.direction().z;
      break;
  }

  return (position - origin) / direction;
}

bool operator ==(const Vector &a, const Vector &b) {
  return a.x == b.x && a.y == b.y && a.z == b.z;
}

bool operator !=(const Vector &a, const Vector &b) {
  return !(a == b);
}

bool operator ==(const Point &a, const Point &b) {
  return (b - a) == Vector{0, 0, 0};
}

bool operator !=(const Point &a, const Point &b) {
  return !(a == b);
}

Vector operator -(const Point &p, const Point &q) {
  return Vector{p.x - q.x, p.y - q.y, p.z - q.z};
}

Point operator +(const Point &p, const Vector &v) {
  return Point{p.x + v.x, p.y + v.y, p.z + v.z};
}

Vector & Vector::operator +=(const Vector &other) {
  x += other.x;
  y += other.y;
  z += other.z;
  return *this;
}

Vector & Vector::operator -=(const Vector &other) {
  x -= other.x;
  y -= other.y;
  z -= other.z;
  return *this;
}

Vector & Vector::operator *=(const Real &a) {
  x *= a;
  y *= a;
  z *= a;
  return *this;
}

Vector & Vector::operator /=(const Real &a) {
  x /= a;
  y /= a;
  z /= a;
  return *this;
}

Vector Vector::operator -() const {
  return Vector{-x, -y, -z};
}

Vector operator +(const Vector &v, const Vector &u) {
  Vector tmp = v;
  return tmp += u;
}

Vector operator -(const Vector &v, const Vector &u) {
  Vector tmp = v;
  return tmp -= u;
}

Vector operator *(const Real &a, const Vector &v) {
  Vector tmp = v;
  return tmp *= a;
}

Vector operator *(const Vector &v, const Real &a) {
  return a * v;
}

Vector operator /(const Vector &v, const Real &a) {
  Vector tmp = v;
  return tmp /= a;
}

Real Dot(const Vector &v, const Vector &u) {
  return v.x * u.x + v.y * u.y + v.z * u.z;
}

Vector Cross(const Vector &v, const Vector &u) {
  return Vector{v.y * u.z - v.z * u.y,
                v.z * u.x - v.x * u.z,
                v.x * u.y - v.y * u.x};
}

Real Length2(const Vector &v) {
  return Dot(v, v);
};

Real Length(const Vector &v) {
  return Length2(v).Sqrt();
};

Vector Normed(const Vector &v) {
  return v / Length(v);
}
