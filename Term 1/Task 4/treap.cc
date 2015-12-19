#include "treap.h"

#include <memory>

#include "order.h"

TreapTree::TreapTree(std::shared_ptr<TreapTree::Node> root) : root_(root) {}

void TreapTree::Insert(int key, int weight) {
  AddNode(root_, std::make_shared<TreapTree::Node>(key, weight));
}

std::size_t TreapTree::Order() const {
  return NodeOrder(root_);
}

void TreapTree::SplitNode(std::shared_ptr<TreapTree::Node> tree, int key,
           std::shared_ptr<TreapTree::Node> &subtree1,
           std::shared_ptr<TreapTree::Node> &subtree2) {
  if (tree == nullptr) {
    subtree1 = subtree2 = nullptr;
    return;
  }

  if (key > tree->key) {
    SplitNode(tree->right, key, tree->right, subtree2);
    subtree1 = tree;
  } else {
    SplitNode(tree->left, key, subtree1, tree->left);
    subtree2 = tree;
  }
}

void TreapTree::AddNode(std::shared_ptr<TreapTree::Node> &tree,
                        std::shared_ptr<TreapTree::Node> node) {
  if (tree) {
    if (node->weight >= tree->weight) {
      SplitNode(tree, node->key, node->left, node->right);
      tree = node;
    } else {
      AddNode((node->key > tree->key ? tree->right : tree->left), node);
    }
  } else {
    tree = node;
  }
}

TreapTree::Node::Node(int key, int weight) : key(key), weight(weight) {}
