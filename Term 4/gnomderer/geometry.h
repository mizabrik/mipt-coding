#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include "real.h"

struct Point {
  Real x;
  Real y;
  Real z;
};

bool operator ==(const Point &a, const Point &b);
bool operator !=(const Point &a, const Point &b);

struct Vector {
  Real x, y, z;

  Vector & operator +=(const Vector &other);
  Vector & operator -=(const Vector &other);
  Vector & operator *=(const Real &c);
  Vector & operator /=(const Real &c);

  Vector operator -() const;
};

bool operator ==(const Vector &a, const Vector &b);
bool operator !=(const Vector &a, const Vector &b);

class Ray {
 public:
  Ray(Point origin, Vector direction);

  Ray(Point origin, Point p);

  Point GetPoint(const Real &distance) const;

  Point origin() const;
  Vector direction() const;

 private:
  Point origin_;
  Vector direction_;
};

Vector operator -(const Point &p, const Point &q);

Point operator +(const Point &p, const Vector &v);

Vector operator +(const Vector &v, const Vector &u);
Vector operator -(const Vector &v, const Vector &u);
Vector operator *(const Real &a, const Vector &u);
Vector operator *(const Vector &u, const Real &a);
Vector operator /(const Vector &u, const Real &a);

Real Dot(const Vector &v, const Vector &u);
Vector Cross(const Vector &v, const Vector &u);

Real Length(const Vector &v);
Real Length2(const Vector &v);

Vector Normed(const Vector &v);

#endif // GEOMETRY_H_
