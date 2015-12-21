#ifndef AVL_H_
#define AVL_H_

#include <memory>

class AvlTree {
 public:
  // Add key into tree (flase, if it's already there)
  bool Insert(int key);

  // Check, if there is key in tree
  bool Has(int key);

  // Remove key (false, if there is no such key)
  bool Remove(int key);

  // Get element, which would take k'th place in sorted array of tree's keys
  int GetOrderStatistic(std::size_t k);

  bool Check() {
    return CheckNode(root_);
  }

 private:
  struct Node {
    Node(int key) : key(key), height(1), size(1) {}

    int key;
    std::size_t height;
    std::size_t size;

    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
  };

  bool CheckNode(std::unique_ptr<Node> &node) {
    if (!node)
      return true;

    if ((!node->left || node->left->key <= node->key)
        && (!node->right || node->right->key > node->key))
      return CheckNode(node->left) && CheckNode(node->right);
    else
      return false;
  }

  // Add node to tre
  static bool AddNode(std::unique_ptr<Node> &tree, std::unique_ptr<Node> &&node);

  // Remove node with such key from tree
  static bool RemoveNode(std::unique_ptr<Node> &tree, int key);

  // Get k'th order statistic in tree
  static int TreeOrderStatistic(std::unique_ptr<Node> &tree, std::size_t k);

  // Remove minimal element from tree (used for deletion)
  static std::unique_ptr<Node> && ExtractMin(std::unique_ptr<Node> &tree);

  // Support tree properties after changes
  static void RestoreBalance(std::unique_ptr<Node> &tree);
  static void UpdateHeight(std::unique_ptr<Node> &node);

  // Wrappers, which return real value or 0 for null
  static std::size_t TreeSize(std::unique_ptr<Node> &node);
  static std::ptrdiff_t TreeDifference(std::unique_ptr<Node> &node);
  static std::size_t TreeHeight(std::unique_ptr<Node> &node);

  // Rotations, used to support tree's balance
  static void LeftRotation(std::unique_ptr<Node> &node);
  static void RightRotation(std::unique_ptr<Node> &node);
  static void BigLeftRotation(std::unique_ptr<Node> &node);
  static void BigRightRotation(std::unique_ptr<Node> &node);

  std::unique_ptr<Node> root_;
};

#endif // AVL_H_
