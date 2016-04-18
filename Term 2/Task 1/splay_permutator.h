#ifndef SPLAY_PERMUTATOR_H_
#define SPLAY_PERMUTATOR_H_

#include "permutator.h"

#include <cstdlib>
#include <cassert>

#include <vector>
#include <memory>

// Data type, which supports inserting, assigning values into array
// and RSQ and next_permutation on range (reverse as a bonus!)
// with time complexity O(log N)
template<typename T>
class SplayPermutator : public PermutatorInterface<T> {
 public:
  SplayPermutator() {
    std::srand(42); // Chosen by fair dice roll, guaranteed to be random
  }

  SplayPermutator(const SplayPermutator &other) = delete;

  SplayPermutator operator =(const SplayPermutator &other) = delete;

  void Insert(unsigned position, T value) {
    assert(position <= TreeSize(root_));
    
    auto subtrees = SplitTree(std::move(root_), position);
    root_ = std::make_unique<Node>(value);
    root_->left = std::move(subtrees.first);
    root_->right = std::move(subtrees.second);

    UpdateTree(*root_);
  }

  unsigned Size() {
    return TreeSize(root_);
  }

  void Set(unsigned position, T value) {
    assert(position < TreeSize(root_));
    SetValue(root_, position, value);
  }

  bool NextPermutation(unsigned begin, unsigned end) {
    assert(begin <= end);
    assert(end <= TreeSize(root_));
    if (begin == end)
      return true;

    auto right_split = SplitTree(std::move(root_), end);
    auto left_split = SplitTree(std::move(right_split.first), begin);
    NodePtr &tree = left_split.second;

    bool success;
    if (tree->suffix.begin == 0) {
      ReverseTree(*tree);
      success = false;
    } else {
      tree = TreeNextPermutation(std::move(tree));
      success = true;
    }

    root_ = MergeTrees(MergeTrees(std::move(left_split.first),
                                  std::move(left_split.second)),
                       std::move(right_split.second));

    return success;
  }

  // Reverse the array
  void Reverse() {
    if (root_)
      ReverseTree(*root_);
  }

  std::vector<T> Dump() {
    std::vector<T> values;

    InOrderDump(*root_, values);
    
    return values;
  }

  void Add(unsigned begin, unsigned end, T value) {
    assert(begin <= end);
    assert(end <= TreeSize(root_));

    if (begin == end)
      return;

    auto right_split = SplitTree(std::move(root_), end);
    auto left_split = SplitTree(std::move(right_split.first), begin);

    IncreaseTree(*left_split.second, value);

    root_ = MergeTrees(MergeTrees(std::move(left_split.first),
                                  std::move(left_split.second)),
                       std::move(right_split.second));
  }

  T GetSum(unsigned begin, unsigned end) {
    assert(begin <= end);
    assert(end <= TreeSize(root_));

    auto right_split = SplitTree(std::move(root_), end);
    auto left_split = SplitTree(std::move(right_split.first), begin);

    T sum = TreeSum(left_split.second);

    root_ = MergeTrees(MergeTrees(std::move(left_split.first),
                                  std::move(left_split.second)),
                       std::move(right_split.second));

    return sum;
  }

 private:
  struct Node;
  using NodePtr = std::unique_ptr<Node>;

  // Used for fast next_permutation
  struct Prefix {
    unsigned end; // interval [0, end)
    T max;
  };
  struct Suffix {
    unsigned begin; // interval [begin, TreeSize)
    T max;
  };

  // Cartesian tree
  struct Node {
    Node(T value)
      : value(value),
        sum(value),
        added(0),
        leftmost(value),
        rightmost(value),
        prefix{1, value},
        suffix{0, value},
        reversed(false),
        size(1) {}

    T value;
    T sum;
    T added;
    T leftmost;
    T rightmost;
    Prefix prefix;
    Suffix suffix;
    bool reversed;

    unsigned size;
    int weight;

    Node *parent;
    NodePtr left;
    NodePtr right;
  };

  std::unique_ptr<Node> root_;
  
  enum class Orientation { kLeft, kRight };

  void ZigTree(NodePtr &tree, Orientation orientation) {
    PushUpdates(*tree);
    NodePtr new_tree;

    switch (orientation) {
      case Orientation::kLeft:
        PushUpdates(*tree->left);

        new_tree = std::move(tree->left);
        tree->left = std::move(new_tree->right);
        new_tree->right = std::move(tree);
        tree = std::move(new_tree);

        UpdateTree(*tree->right);
        break;

      case Orientation::kRight:
        PushUpdates(*tree->right);

        new_tree = std::move(tree->right);
        tree->right = std::move(new_tree->left);
        new_tree->left = std::move(tree);
        tree = std::move(new_tree);

        UpdateTree(*tree->left);
        break;
    }

    UpdateTree(*tree);
  }

  void ZigZigTree(NodePtr &tree, Orientation orientation) {
      ZigTree(tree, orientation);
      ZigTree(tree, orientation);
  }

  void ZigZagTree(NodePtr &tree, Orientation orientation) {
    PushUpdates(*tree);
    switch (orientation) {
      case Orientation::kLeft:
        ZigTree(tree->left, Orientation::kRight);
        ZigTree(tree, orientation);
        break;

      case Orientation::kRight:
        ZigTree(tree->right, Orientation::kLeft);
        ZigTree(tree, orientation);
        break;
    }
  }

  void Splay(std::reference_wrapper<NodePtr> tree, unsigned k) {
    assert(tree.get() != nullptr);

    using PathStep = std::pair<std::reference_wrapper<NodePtr>, Orientation>;

    std::vector<PathStep> path;
    PushUpdates(*tree.get());
    while (TreeSize(tree.get()->left) != k) {
      if (k < TreeSize(tree.get()->left)) {
        path.emplace_back(tree, Orientation::kLeft);
        PushUpdates(*tree.get()->left);
        tree = tree.get()->left;
      } else {
        k -= TreeSize(tree.get()->left) + 1;
        path.emplace_back(tree, Orientation::kRight);
        PushUpdates(*tree.get()->right);
        tree = tree.get()->right;
      }
    }

    while (path.size() > 1) {
      PathStep dad = path.back();
      path.pop_back();
      PathStep grandpa = path.back();
      path.pop_back();

      if (dad.second == grandpa.second) {
        ZigZigTree(grandpa.first.get(), grandpa.second);
      } else {
        ZigZagTree(grandpa.first.get(), grandpa.second);
      }
    }

    if (path.size())
      ZigTree(path[0].first.get(), path[0].second);
  }

  NodePtr MergeTrees(NodePtr tree1, NodePtr tree2) {
    if (tree1 == nullptr)
      return tree2;

    Splay(tree1, TreeSize(tree1) - 1);
    tree1->right = std::move(tree2);
    UpdateTree(*tree1);

    return tree1;
  }

  std::pair<NodePtr, NodePtr> SplitTree(NodePtr tree, unsigned n) {
    if (n == 0)
      return std::make_pair(NodePtr(nullptr), std::move(tree));

    Splay(tree, n - 1);
    std::pair<NodePtr, NodePtr> result;
    PushUpdates(*tree);
    result.second = std::move(tree->right);
    UpdateTree(*tree);
    result.first = std::move(tree);
    return result;
  }

  T SetValue(NodePtr &tree, unsigned position, T value) {
    Splay(tree, position);
    T old = tree->value;
    tree->value = value;

    UpdateTree(*tree);

    return old;
  }

  // Node parametrs access helpers
  unsigned TreeSize(const NodePtr &tree) {
    return tree ? tree->size : 0;
  }
  int TreeSum(const NodePtr &tree) {
    return tree ? tree->sum : 0;
  }

  void InOrderDump(Node &tree, std::vector<T> &values) {
    PushUpdates(tree);

    if (tree.left)
      InOrderDump(*tree.left, values);
    values.push_back(tree.value);
    if (tree.right)
      InOrderDump(*tree.right, values);
  }

  // Updates subtree information, supposes both subtrees are coreect.
  // Must be called after any tree modification.
  void UpdateTree(Node &tree) {
    tree.size = TreeSize(tree.left) + 1 + TreeSize(tree.right);
    tree.sum = TreeSum(tree.left) + tree.value + TreeSum(tree.right);
    tree.leftmost = tree.left ? tree.left->leftmost : tree.value;
    tree.rightmost = tree.right ? tree.right->rightmost : tree.value;

    Prefix prefix = tree.left ? tree.left->prefix : Prefix{0, tree.value};
    if (prefix.end == TreeSize(tree.left) && prefix.max <= tree.value) {
      prefix.end += 1;
      prefix.max = tree.value;
      if (tree.right && prefix.max <= tree.right->leftmost) {
        prefix.end += tree.right->prefix.end;
        prefix.max = tree.right->prefix.max;
      }
    }
    tree.prefix = prefix;

    Suffix suffix = tree.right ? tree.right->suffix : Suffix{0, tree.value};
    suffix.begin += TreeSize(tree.left) + 1;
    if (suffix.begin == TreeSize(tree.left) + 1 && tree.value >= suffix.max) {
        suffix.begin -= 1;
        suffix.max = tree.value;
        if (tree.left && tree.left->rightmost >= suffix.max) {
          suffix = tree.left->suffix;
        }
    }
    tree.suffix = suffix;
  }

  // Starts postponed reverse; before working with a node one
  // should "push" the reverse to it's children
  void ReverseTree(Node &tree) {
    tree.reversed ^= true;
    auto suffix = Suffix{tree.size - tree.prefix.end, tree.prefix.max};
    tree.prefix = Prefix{tree.size - tree.suffix.begin, tree.prefix.max};
    tree.suffix = suffix;
    std::swap(tree.leftmost, tree.rightmost);
  }

  void IncreaseTree(Node &tree, T value) {
    tree.value += value;
    tree.sum += tree.size * value;
    tree.added = value;
    tree.leftmost += value;
    tree.rightmost += value;
    tree.prefix.max += value;
    tree.suffix.max += value;
  }

  // Move reversed state from node to it's children to do the actual reverse
  // (on one level)
  void PushUpdates(Node &tree) {
    if (tree.reversed) {
      std::swap(tree.left, tree.right);
      tree.reversed = false;
      if (tree.left)
        ReverseTree(*tree.left);
      if (tree.right)
        ReverseTree(*tree.right);
    }

    if (tree.added) {
      if (tree.left)
        IncreaseTree(*tree.left, tree.added);
      if (tree.right)
        IncreaseTree(*tree.right, tree.added);
      tree.added = 0;
    }
  }

  NodePtr TreeNextPermutation(NodePtr tree) {
    auto suffix_split = SplitTree(std::move(tree), tree->suffix.begin);
    NodePtr &suffix = suffix_split.second;
    NodePtr &node = suffix_split.first;
    Splay(node, TreeSize(node) - 1);

    ReverseTree(*suffix);
    node->value = SetValue(suffix,
                          FindBiggerThan(*suffix, node->value), node->value);
    UpdateTree(*node);

    return MergeTrees(std::move(node), std::move(suffix));
  }

  // Used for NextPermutation; supposes, that values are sorted.
  unsigned FindBiggerThan(Node &tree, T value) {
    PushUpdates(tree);

    if (tree.value > value) {
      return tree.left ? FindBiggerThan(*tree.left, value) : 0;
    } else if (tree.right) {
      return TreeSize(tree.left) + 1 + FindBiggerThan(*tree.right, value);
    } else {
      return tree.size;
    }
    return 0;
  }
};

#endif // SPLAY_PERMUTATOR_H_
