#ifndef REAL_H_
#define REAL_H_

#include <iostream>

static const double EPS = 1e-6;

class Real {
 public:
  Real(double value = 0.0);

  double value() const;

  int sign() const;

  Real abs() const;

  Real Sqrt() const;

  Real & operator +=(const Real &other);
  Real & operator -=(const Real &other);
  Real & operator *=(const Real &other);
  Real & operator /=(const Real &other);

  Real operator -() const;

 private:
  double value_;
};

Real operator +(const Real &a, const Real &b);
Real operator -(const Real &a, const Real &b);
Real operator *(const Real &a, const Real &b);
Real operator /(const Real &a, const Real &b);

bool operator ==(const Real &a, const Real &b);
bool operator !=(const Real &a, const Real &b);
bool operator <=(const Real &a, const Real &b);
bool operator >=(const Real &a, const Real &b);
bool operator <(const Real &a, const Real &b);
bool operator >(const Real &a, const Real &b);

std::ostream& operator<< (std::ostream& s, const Real& x);

#endif // REAL_H_
