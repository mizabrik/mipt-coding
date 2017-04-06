#include "color.h"

#include <algorithm>
#include <cmath>

Color & operator *=(Color &color, double x) {
  color.r = std::min(255LL, (long long) std::round(color.r * x));
  color.g = std::min(255LL, (long long) std::round(color.g * x));
  color.b = std::min(255LL, (long long) std::round(color.b * x));
}

Color operator *(const Color &color, double x) {
  Color tmp = color;
  return tmp *= x;
}

Color operator *(double x, const Color &color) {
  return color * x;
}
