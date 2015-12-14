#ifndef NAIVE_H_
#define NAIVE_H_

#include <memory>

class NaiveTree {
 public:
  void Add(int key) {
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

  size_t Order() {
    return NodeOrder(root_);
  }

 private:
  struct Node {
    Node(int key) : key(key) {}

    int key;
    std::shared_ptr<Node> left;
    std::shared_ptr<Node> right;
  };

  size_t NodeOrder(std::shared_ptr<Node> node) {
    std::size_t order = 0;

    if (node != nullptr)
      order = 1 + std::max(NodeOrder(node->left), NodeOrder(node->right));

    return order;
  }

  std::shared_ptr<Node> root_;
};

#endif // NAIVE_H_
