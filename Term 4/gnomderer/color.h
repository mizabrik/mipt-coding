#ifndef COLOR_H_
#define COLOR_H_

#include <SFML/Graphics/Color.hpp>

using Color = sf::Color;

Color & operator *=(Color &color, double x);

Color operator *(const Color &color, double x);
Color operator *(double x, const Color &color);

#endif // COLOR_H_
