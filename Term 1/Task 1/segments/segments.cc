#include "segments.h"

#include "array.h"

inline bool operator<(EndPoint a, EndPoint b) {
  return a.first < b.first;
}

inline bool operator<=(EndPoint a, EndPoint b) {
  return a.first <= b.first;
}

inline bool operator>(EndPoint a, EndPoint b) {
  return a.first > b.first;
}

inline bool operator>=(EndPoint a, EndPoint b) {
  return a.first >= b.first;
}

double get_nested_1_length(Array<EndPoint> end_points) {
  end_points.Sort();

  double result = 0;
  int nested_level = 0;
  double matching_point = 0;

  for (int i = 0; i < end_points.length(); i++) {
    EndPoint end_point = end_points[i];

    if (nested_level == 1)
      result += end_point.first - matching_point;

    if (end_point.second == kLeft)
      ++nested_level;
    else
      --nested_level;

    if (nested_level == 1)
      matching_point = end_point.first;
  }

  return result;
}
