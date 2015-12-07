#include <iostream>

#include "rectangles.h"

int main() {
  MaxAreaCounter max_area;

  unsigned n;
  std::cin >> n;
  for (unsigned i = 0; i < n; ++i) {
    unsigned width, height;
    std::cin >> width >> height;
    max_area.Add(width, height);
  }

  std::cout << max_area << std::endl;

  return 0;
}
