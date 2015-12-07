#include <iostream>
#include <iterator>
#include <memory>

#include "xml.h"

void Print(xml::Element &element, size_t offset = 0);

int main() {
  auto result = std::unique_ptr<xml::Element>(xml::Parse(std::istream_iterator<char>(std::cin)));

  Print(*result);

  return 0;
}

void Print(xml::Element &element, size_t offset) {
  std::cout << element.name << std::endl;

  for (auto &child : element.children) {
    if (child->type == xml::Node::Type::kText) {
      std::cout << std::string(" ", offset) << "├"
          << static_cast<xml::Text &>(*child).text << std::endl;
    } else {
      std::cout << std::string(" ", offset) << "╞";

      Print(static_cast<xml::Element &>(*child), offset + 1);
    }
  }
}
