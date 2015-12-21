#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>

#include "sparse.h"

int main() {
  std::size_t size, queries;
  std::cin >> size >> queries;

  std::vector<int> data;

  std::copy_n(std::istream_iterator<int>(std::cin), size,
              std::back_inserter(data));

  SparseTable st(std::move(data));

  while(queries--) {
    std::size_t left, right;
    std::cin >> left >> right;
    std::cout << st.Query(left - 1, right - 1) << std::endl;
  }

  return 0;
}
