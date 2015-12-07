#ifndef SEGMENTS_SEGMENTS_H_
#define SEGMENTS_SEGMENTS_H_

#include "array.h"

enum EndPointTypes {
  kLeft,
  kRight
};

template<typename T1, typename T2>
struct Pair {
  T1 first;
  T2 second;
};

using EndPoint = Pair<double, EndPointTypes>;

inline bool operator<(EndPoint a, EndPoint b);
inline bool operator<=(EndPoint a, EndPoint b);
inline bool operator>(EndPoint a, EndPoint b);
inline bool operator>=(EndPoint a, EndPoint b);

double get_nested_1_length(Array<EndPoint> end_points);

#endif // SEGMENTS_SEGMENTS_H_
