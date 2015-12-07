#ifndef SKEW_HEAP_H_
#define SKEW_HEAP_H_

#include "heap.h"

#include <cassert>

#include <memory>

// Skew/Leftist heap is a binary tree, which is rotated to minimize
// length of way from root to leaves
template <typename T, bool is_leftist = false>
class SkewHeap : public HeapInterface<T> {
 public:
  SkewHeap() : size_(0) {}

  explicit SkewHeap(T &&key)
      : size_(1),
        root_(new SkewNode(std::move(key))) {}

  void Insert(T &&key) {
    SkewHeap<T, is_leftist> tmp = SkewHeap<T, is_leftist>(std::move(key));
    Meld(&tmp);
  }

  const T & GetMinimum() {
    assert(size() != 0);

    return root_->key;
  }

  T && ExtractMinimum() {
    assert(size() != 0);

    NodePtr old_root = std::move(root_);
    T key = old_root->key;
    --size_;

    root_ = MeldNodes(std::move(old_root->left), std::move(old_root->right));

    return std::move(key);
  }

  void Meld(HeapInterface<T> *other_typeless) {
    SkewHeap *other = dynamic_cast<SkewHeap<T, is_leftist> *>(other_typeless);
    assert(other != nullptr);

    root_ = MeldNodes(std::move(root_), std::move(other->root_));

    size_ += other->size_;
    other->size_ = 0;
  }

  std::size_t size() {
    return size_;
  }

 protected:
  struct SkewNode;
  using NodePtr = std::unique_ptr<SkewNode>;

  // Actual tree structure
  struct SkewNode {
    SkewNode(int &&key) : key(std::move(key)), order(0) {}

    T key;
    std::size_t order;

    NodePtr left;
    NodePtr right;
  };

  // Meld two trees into one
  NodePtr MeldNodes(NodePtr a, NodePtr b) {
    if (a == nullptr)
      return b;
    if (b == nullptr)
      return a;

    if (b->key < a->key)
      std::swap(a, b);

    a->right = MeldNodes(std::move(a->right), std::move(b));

    Rotate(a);
    a->order = std::min(a->left->order, a->right ? a->right->order : 0) + 1;
    
    return a;
  }

  // Rotate node's children if needed
  void Rotate(NodePtr &node) {
    if (!is_leftist || !node->left || node->left->order > node->right->order)
      std::swap(node->left, node->right);
  }

  std::size_t size_;
  NodePtr root_;
};

template <typename T>
using LeftistHeap = SkewHeap<T, true>;

#endif // SKEW_HEAP_H_
