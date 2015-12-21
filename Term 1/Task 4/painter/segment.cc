#include "segment.h"

#include <vector>
#include <limits>

SegmentTree::SegmentTree(std::vector<int> &array)
  : data_(4 * array.size(), Value(kNeutral)),
    root_(data_, 0, 0, array.size() - 1) {
  BuildTree(array, root_);
}

int SegmentTree::GetMin(std::size_t begin, std::size_t end) {
  return root_.Min(begin, end);
}

void SegmentTree::BuildTree(std::vector<int> &array, Node tree) {
  if (tree.begin() == tree.end()) {
    tree.value() = array[tree.begin()];
  } else {
    BuildTree(array, tree.left());
    BuildTree(array, tree.right());
    tree.value() = std::min(tree.left().value(), tree.right().value());
  }
}

void SegmentTree::UpdateSegment(std::size_t begin, std::size_t end, int value) {
  root_.Update(begin, end, value);
}

SegmentTree::Value::Value(int value) : value(value), updated(false) {}

SegmentTree::Node::Node(std::vector<Value> &data, std::size_t id,
                        std::size_t begin, std::size_t end)
  : data_(data),
    id_(id),
    begin_(begin),
    end_(end) {}

SegmentTree::Node SegmentTree::Node::left() {
  return Node(data_, id_ * 2 + 1, begin_, middle()); 
}

SegmentTree::Node SegmentTree::Node::right() {
  return Node(data_, id_ * 2 + 2, middle() + 1, end_); 
}

std::size_t SegmentTree::Node::begin() {
  return begin_;
}

std::size_t SegmentTree::Node::end() {
  return end_;
}

int SegmentTree::Node::Min(std::size_t begin, std::size_t end) {
  if (begin > end)
    return kNeutral;

  if (begin == this->begin() && end == this->end()) {
    return value();
  }

  Push();
  return std::min(left().Min(begin, std::min(end, middle())),
                  right().Min(std::max(middle() + 1, begin), end));
}

void SegmentTree::Node::Update(std::size_t begin, std::size_t end, int value) {
  if (begin > end)
    return;

  if (begin == this->begin() && end == this->end()) {
    this->value() = value;
    data_[id_].updated = true;
  } else {
    left().Update(begin, std::min(end, middle()), value);
    right().Update(std::max(middle() + 1, begin), end, value);
  }
}

void SegmentTree::Node::Push() {
  if (begin() != end() && updated()) {
    left().value() = right().value() = value();
    data_[left().id_].updated = data_[right().id_].updated = true;
    data_[id_].updated = false;
  }
}

int & SegmentTree::Node::value() {
  return data_[id_].value;
}

std::size_t SegmentTree::Node::middle() {
  return (begin_ + end_) / 2;
}

const int SegmentTree::kNeutral = std::numeric_limits<int>::max();
