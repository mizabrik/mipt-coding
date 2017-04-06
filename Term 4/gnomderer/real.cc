#include "real.h"

#include <cmath>
#include <iostream>

Real::Real(double value) : value_(value) {};

double Real::value() const {
  return value_;
}

int Real::sign() const {
  if (value_ < -EPS) {
    return -1;
  } else if (value_ > EPS) {
    return 1;
  } else {
    return 0;
  }
}

Real Real::abs() const {
  return sign() * *this;
}

Real Real::Sqrt() const {
  return std::sqrt(value_);
};

Real Real::operator -() const {
  return -value();
}

bool operator ==(const Real &a, const Real &b);

Real & Real::operator +=(const Real &other) {
  value_ += other.value();
  return *this;
}

Real & Real::operator -=(const Real &other) {
  value_ -= other.value();
  return *this;
}

Real & Real::operator *=(const Real &other) {
  value_ *= other.value();
  return *this;
}

Real & Real::operator /=(const Real &other) {
  value_ /= other.value();
  return *this;
}


Real operator +(const Real &a, const Real &b) {
  Real tmp = a;
  return tmp += b;
}

Real operator -(const Real &a, const Real &b) {
  Real tmp = a;
  return tmp -= b;
}

Real operator *(const Real &a, const Real &b) {
  Real tmp = a;
  return tmp *= b;
}

Real operator /(const Real &a, const Real &b) {
  Real tmp = a;
  return tmp /= b;
}


bool operator ==(const Real &a, const Real &b) {
  return (a - b).sign() == 0;
}

bool operator !=(const Real &a, const Real &b) {
  return (a - b).sign() != 0;
}

bool operator <=(const Real &a, const Real &b) {
  return (a - b).sign() <= 0;
}

bool operator >=(const Real &a, const Real &b) {
  return (a - b).sign() >= 0;
}

bool operator <(const Real &a, const Real &b) {
  return (a - b).sign() < 0;
}

bool operator >(const Real &a, const Real &b) {
  return (a - b).sign() > 0;
}

std::ostream& operator<< (std::ostream& s, const Real& x) {
  return s << x.value();
}
