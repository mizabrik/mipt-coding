#include "rectangles.h"

#include <cstdint>

#include <vector>

MaxAreaCounter::MaxAreaCounter() : position_(0), maximum_(0) {}

void MaxAreaCounter::Add(uint64_t width, uint64_t height) {
  uint64_t start = position_;
  
  while (rectangles_.size() && rectangles_.back().height >= height) {
    start = rectangles_.back().start;
    UpdateMaximum(rectangles_.back().start, rectangles_.back().height);
    rectangles_.pop_back();
  }

  rectangles_.push_back(Rectangle{start, height});
  position_ += width;
}

uint64_t MaxAreaCounter::GetMaximum() {
  for (auto rectangle : rectangles_)
    UpdateMaximum(rectangle.start, rectangle.height);

  return maximum_;
}

std::ostream & operator<<(std::ostream &output, MaxAreaCounter area) {
  output << area.GetMaximum();
  return output;
}
void MaxAreaCounter::UpdateMaximum(uint64_t start, uint64_t height) {
  uint64_t square = height * (position_ - start);
  if (square > maximum_)
    maximum_ = square;
}
