#include <iostream>
#include <cstdlib>

#include "avl.h"

int main() {
  AvlTree tree;

  std::size_t count;
  std::cin >>  count;

  for (std::size_t i = 0; i < count; ++i) {
    int number;
    std::cin >> number;
    if (number >= 0) {
      tree.Insert(number);
    } else {
      tree.Remove(-number);
    }

    std::size_t statistic;
    std::cin >> statistic;
    std::cout << tree.GetOrderStatistic(statistic) << std::endl;
  }

    int seed;
    std::cin >> seed;
    srand(seed);
  while(count) {
    bool add = rand() % 2;
    int number = rand() % 10 + 1;
    if (add) {
      std::cout << '+' << number << std::endl;
      tree.Insert(number);
    } else {
      std::cout << '-' << number << std::endl;
      tree.Remove(number);
    }
    std::cout << tree.Check() << std::endl;
    --count;
  }

  return 0;
}
