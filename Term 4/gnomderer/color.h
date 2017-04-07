#ifndef COLOR_H_
#define COLOR_H_

#include <iterator>
#include <SFML/Graphics/Color.hpp>

using Color = sf::Color;

Color & operator *=(Color &color, double x);

Color operator *(const Color &color, double x);
Color operator *(double x, const Color &color);

template <typename It>
Color Average(It begin, It end) {
  unsigned int r = 0, g = 0, b = 0;
  unsigned int n = 0;

  while (begin != end) {
    ++n;
    r += begin->r;
    g += begin->g;
    b += begin->b;
    ++begin;
  }

  if (n) {
    r /= n;
    g /= n;
    b /= n;
  }

  return Color(r, g, b);
}

#endif // COLOR_H_
