#include <iostream>
#include <vector>

#include "segment.h"

int main() {
  std::size_t length;
  std::cin >> length;
  std::vector<int> data;

  for (std::size_t i = 0; i < length; ++i) {
    int red, green, blue;
    std::cin >> red >> green >> blue;
    data.push_back(red + green + blue);
  }

  SegmentTree tree(data);

  std::size_t queries;
  std::cin >> queries;

  for (std::size_t i = 0; i < queries; ++i) {
    std::size_t begin, end;
    std::cin >> begin >> end;
    int red, green, blue;
    std::cin >> red >> green >> blue;
    tree.UpdateSegment(begin, end, red + green + blue);

    std::cin >> begin >> end;
    std::cout << tree.GetMin(begin, end) << std::endl;
  }
}
