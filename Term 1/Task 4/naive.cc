#include "naive.h"

#include <memory>

#include "order.h"

void NaiveTree::Insert(int key) {
  if (root_ == nullptr) {
    root_ = std::make_shared<Node>(key);
    return;
  }

  std::shared_ptr<Node> parent = root_;
  do {
    std::shared_ptr<Node> &new_parent  = key > parent->key
                                       ? parent->right : parent->left;
    if (new_parent == nullptr) {
      new_parent = std::make_shared<Node>(key);
      return;
    }

    parent = new_parent;
  } while (true);
}

size_t NaiveTree::Order() const {
  return NodeOrder(root_);
}

NaiveTree::Node::Node(int key) : key(key) {}
