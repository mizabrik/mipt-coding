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
