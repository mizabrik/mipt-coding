#ifndef TREAP_H_
#define TREAP_H_

#include <memory>

#include "order.h"

class TreapTree {
 public:
  TreapTree() = default;

  TreapTree(const TreapTree &other) = delete;
  TreapTree & operator =(const TreapTree &other) = delete;

  TreapTree(TreapTree &&other) = default;
  TreapTree & operator =(TreapTree &&other) = default;

  void Insert(int key, int weight);

  std::size_t Order() const;

 private:
  struct Node {
    Node(int key, int order);

    int key;
    int weight;

    std::shared_ptr<Node> left;
    std::shared_ptr<Node> right;
  };

  friend std::size_t NodeOrder<>(const std::shared_ptr<TreapTree::Node> node);

  TreapTree(std::shared_ptr<TreapTree::Node> root);

  static void SplitNode(std::shared_ptr<TreapTree::Node> tree, int key,
                 std::shared_ptr<TreapTree::Node> &subtree1,
                 std::shared_ptr<TreapTree::Node> &subtree2);

  static void AddNode(std::shared_ptr<TreapTree::Node> &tree,
                          std::shared_ptr<TreapTree::Node> node);

  std::shared_ptr<Node> root_;
};

#endif // TREAP_H_
