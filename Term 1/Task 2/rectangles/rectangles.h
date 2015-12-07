#ifndef RECTANGLES_H_
#define RECTANGLES_H_

#include <cstdint>

#include <vector>
#include <iostream>

class MaxAreaCounter {
  friend std::ostream & operator<<(std::ostream &output, MaxAreaCounter area);
 public:
  MaxAreaCounter();

  // Append new rectagle after last
  void Add(uint64_t width, uint64_t height); 

  // Get maximal square of inscribed rectangle
  uint64_t GetMaximum();

 private:
  struct Rectangle {
    uint64_t start;
    uint64_t height;
  };

  // Set new maximum to area of rectangle of height height,
  // with x_1 = start and x_2 = position_, if nescessary
  void UpdateMaximum(uint64_t start, uint64_t height); 

  std::vector<Rectangle> rectangles_;
  uint64_t position_;
  uint64_t maximum_;
};

std::ostream & operator<<(std::ostream &output, MaxAreaCounter area);

#endif // RECTANGLES_H_
