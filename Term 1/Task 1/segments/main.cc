// Copyright Alexander Vasilyev

#include <iostream>

#include "array.h"
#include "segments.h"

int main() {
  Array<EndPoint> end_points;

  int segments_num;
  std::cin >> segments_num;

  for (int i = 0; i < segments_num; ++i) {
    double left, right;
    std::cin >> left >> right;
    end_points.Push(EndPoint{left, kLeft});
    end_points.Push(EndPoint{right, kRight});
  }

  std::cout << get_nested_1_length(end_points) << std::endl;

  return 0;
}
