#include "avl.h"

#include <cassert>

#include <memory>

bool AvlTree::Insert(int key) {
  return AddNode(root_, std::make_unique<AvlTree::Node>(key));
}

bool AvlTree::Has(int key) {
  const Node *node = root_.get();
  while(node) {
    if (key < node->key) {
      node = node->left.get();
    } else if (key > node->key) {
      node = node->right.get();
    } else {
      return true;
    }
  }

  return false;
}

bool AvlTree::Remove(int key) {
  return RemoveNode(root_, key);
}

int AvlTree::GetOrderStatistic(std::size_t k) {
  assert(k < TreeSize(root_));
  return TreeOrderStatistic(root_, k);
}
  
bool AvlTree::AddNode(std::unique_ptr<AvlTree::Node> &tree, std::unique_ptr<Node> &&node) {
  bool added = false;

  if (tree == nullptr) {
    tree = std::move(node);
    return true;
  } else if (node->key < tree->key) {
    added = AddNode(tree->left, std::move(node));
  } else if (node->key > tree->key) {
    added = AddNode(tree->right, std::move(node));
  }

  if (added) {
    ++tree->size;
    RestoreTree(tree);
  }

  return added;
}

bool AvlTree::RemoveNode(std::unique_ptr<AvlTree::Node> &tree, int key) {
  if (tree == nullptr)
    return false;

  if (key < tree->key) {
    if (!RemoveNode(tree->left, key))
      return false;
    --tree->size;
  } else if (key > tree->key) {
    if (!RemoveNode(tree->right, key))
      return false;
    --tree->size;
  } else {
    if (tree->right == nullptr) {
      tree = std::move(tree->left);
    } else {
      auto node = ExtractMin(tree->right);
      node->left = tree->left ? std::move(tree->left) : nullptr;
      node->right = std::move(tree->right);
      node->size = TreeSize(tree) - 1;

      tree = std::move(node);
    }
  }
  RestoreTree(tree);

  return true;
}

int AvlTree::TreeOrderStatistic(std::unique_ptr<Node> &tree, std::size_t k) {
  if (k < TreeSize(tree->left)) {
    return TreeOrderStatistic(tree->left, k);
  } else if(k > TreeSize(tree->left)) {
    return TreeOrderStatistic(tree->right, k - (TreeSize(tree->left) + 1));
  } else {
    return tree->key;
  }
}

std::unique_ptr<AvlTree::Node> AvlTree::ExtractMin(std::unique_ptr<Node> &tree) {
  if (tree->left) {
    auto node = ExtractMin(tree->left);
    --tree->size;
    RestoreTree(tree);
    return std::move(node);
  } else {
    auto min = std::move(tree);
    tree = std::move(min->right);
    return min;
  }
}

void AvlTree::RestoreTree(std::unique_ptr<AvlTree::Node> &tree) {
  UpdateHeight(tree);

  switch (TreeDifference(tree)) {
    case -2:
      if (TreeDifference(tree->right) < 1) {
        LeftRotation(tree);
      } else {
        BigLeftRotation(tree);
      }
      break;

    case 2:
      if (TreeDifference(tree->left) > -1) {
        RightRotation(tree);
      } else {
        BigRightRotation(tree);
      }
      break;

    default:
      break;
  }
}

std::size_t AvlTree::TreeSize(std::unique_ptr<AvlTree::Node> &node) {
  return node ? node->size : 0;
}

std::ptrdiff_t AvlTree::TreeDifference(std::unique_ptr<AvlTree::Node> &node) {
  return node ? TreeHeight(node->left) - TreeHeight(node->right) : 0;
}
  
std::size_t AvlTree::TreeHeight(std::unique_ptr<AvlTree::Node> &node) {
  return node ? node->height : 0;
}

void AvlTree::UpdateHeight(std::unique_ptr<AvlTree::Node> &node) {
  if (node)
    node->height = std::max(TreeHeight(node->left),
                            TreeHeight(node->right)) + 1;
}

void AvlTree::LeftRotation(std::unique_ptr<AvlTree::Node> &tree) {
  tree->size -= TreeSize(tree->right);
  tree->right->size += TreeSize(tree);
  tree->size += TreeSize(tree->right->left);

  auto tmp = std::move(tree);
  tree = std::move(tmp->right);
  tmp->right = std::move(tree->left);
  tree->left = std::move(tmp);

  UpdateHeight(tree->left);
  UpdateHeight(tree);
}

void AvlTree::RightRotation(std::unique_ptr<AvlTree::Node> &tree) {
  tree->size -= TreeSize(tree->left);
  tree->left->size += TreeSize(tree);
  tree->size += TreeSize(tree->left->right);

  auto tmp = std::move(tree);
  tree = std::move(tmp->left);
  tmp->left = std::move(tree->right);
  tree->right = std::move(tmp);

  UpdateHeight(tree->right);
  UpdateHeight(tree);
}

void AvlTree::BigLeftRotation(std::unique_ptr<AvlTree::Node> &node) {
  RightRotation(node->right);
  LeftRotation(node);
}

void AvlTree::BigRightRotation(std::unique_ptr<AvlTree::Node> &node) {
  LeftRotation(node->left);
  RightRotation(node);
}
