#ifndef NAIVE_H_
#define NAIVE_H_

#include <memory>

#include "order.h"

class NaiveTree {
 public:
  void Insert(int key);

  size_t Order() const;

 private:
  struct Node {
    Node(int key);

    int key;
    std::shared_ptr<Node> left;
    std::shared_ptr<Node> right;
  };

  friend std::size_t NodeOrder<>(const std::shared_ptr<Node> node);

  std::shared_ptr<Node> root_;
};

#endif // NAIVE_H_
