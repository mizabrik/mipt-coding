#include <iostream>

#include "calc.h"

int main() {
  while (true) {
    std::string expression;

    std::getline(std::cin, expression);

    if (std::cin.eof())
      break;

    std::cout << Calculate(expression) << std::endl;
  }

  return 0;
}
