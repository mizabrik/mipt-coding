#include <iostream>

#include "vector.h"

int main() {
  Vector<int> vector;
  char command;
  size_t index;
  int value;

  while (true) {
    std::cout << "[size=" << vector.size()
              << ",capacity=" << vector.capacity() << "]" << std::endl;

    std::cin >> command;
    
    if(std::cin.eof())
      break;

    switch (command) {
      case '+':
        std::cin >> value;
        vector.PushBack(value);
        break;

      case '-':
        std::cout << vector.PopBack() << std::endl;
        break;

      case '?':
        std::cin >> index;
        std::cout << vector[index] << std::endl;
        break;

      case '=':
        std::cin >> index >> value;
        vector[index] = value;
        break;

      default:
        std::cout << "Supported operations are + NUM (PushBack), - (PopBack),"
                  << "? INDEX (view element)," << std::endl
                  <<  "and = INDEX NUM (set element)" << std::endl;
        break;
    }
  }

  return 0;
}
