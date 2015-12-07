#include <iostream>
#include <string>

#include "hash_table.h"

void print(bool value) {
  std::cout << (value ? "OK" : "FAIL") << std::endl;
}

int main() {
  HashTable table;
  
  char command;
  while(std::cin >> command) {
    std::string key;
    std::getline(std::cin, key);

    switch (command) {
      case '?':
        print(table.Has(key));
        break;
      case '+':
        print(table.Add(key));
        break;
      case '-':
        print(table.Remove(key));
        break;
    }
  }

  return 0;
}
