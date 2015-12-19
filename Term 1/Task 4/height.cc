#include <iostream>

#include "naive.h"
#include "treap.h"

int main() {
  int count;
  std::cin >> count;

  TreapTree treap;
  NaiveTree naive;

  for (int i = 0; i < count; ++i) {
    int key, weight;
    std::cin >> key >> weight;
    treap.Insert(key, weight);
    naive.Insert(key);
  }

  std::cout << (naive.Order() - treap.Order()) << std::endl;
}
