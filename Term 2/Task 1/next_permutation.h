#ifndef NEXT_PERMUTATION_H_
#define NEXT_PERMUTATION_H_

#include <cstdlib>
#include <cassert>

#include <vector>
#include <memory>
#include <type_traits>

// Data type, which supports inserting, assigning values into array
// and RSQ and next_permutation on range (reverse as a bonus!)
// with time complexity O(log N)
template<typename T>
class NextPermutator {
  static_assert(std::is_arithmetic<T>::value,
                "Only arithmetic values are supported");
 public:
  NextPermutator() {
    std::srand(42); // Chosen by fair dice roll, guaranteed to be random
  }

  NextPermutator(const NextPermutator &other) = delete;

  NextPermutator operator =(const NextPermutator &other) = delete;

  // Insert value at given position
  void Insert(unsigned position, T value) {
    assert(position <= TreeSize(root_));
    
    auto subtrees = SplitTree(std::move(root_), position);
    root_ = MergeTrees(std::move(subtrees.first),
                       std::make_unique<Node>(value, std::rand()));
    root_ = MergeTrees(std::move(root_), std::move(subtrees.second));
  }

  // Assign value at given position
  void Set(unsigned position, T value) {
    assert(position < TreeSize(root_));
    SetValue(*root_, position, value);
  }

  // Change interval [begin, end) to it's next permutation
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

  // Dump current array values into vector
  std::vector<T> Dump() {
    std::vector<T> values;

    InOrderDump(*root_, values);
    
    return values;
  }

  // Get RSQ on [begin, end)
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
    Node(T value, int weight)
      : value(value),
        sum(value),
        size(1),
        leftmost(value),
        rightmost(value),
        prefix{1, value},
        suffix{0, value},
        reversed(false),
        weight(weight) {}

    T value;
    T sum;
    T leftmost;
    T rightmost;
    Prefix prefix;
    Suffix suffix;
    bool reversed;

    int size;
    int weight;

    NodePtr left;
    NodePtr right;
  };

  std::unique_ptr<Node> root_;

  NodePtr MergeTrees(NodePtr tree1, NodePtr tree2) {
    if (tree1 == nullptr)
      return tree2;
    if (tree2 == nullptr)
      return tree1;

    if (tree1->weight >= tree2->weight) {
      PushReverse(*tree1);
      tree1->right = MergeTrees(std::move(tree1->right), std::move(tree2));
      UpdateTree(*tree1);
      return tree1;
    } else {
      PushReverse(*tree2);
      tree2->left = MergeTrees(std::move(tree1), std::move(tree2->left));
      UpdateTree(*tree2);
      return tree2;
    }
  }

  std::pair<NodePtr, NodePtr> SplitTree(NodePtr tree, unsigned n) {
    if (n == 0) {
      return std::make_pair(NodePtr(nullptr), std::move(tree));
    }

    PushReverse(*tree);

    if (n <= TreeSize(tree->left)) {
      auto left_split = SplitTree(std::move(tree->left), n);
      tree->left = std::move(left_split.second);
      UpdateTree(*tree);
      left_split.second = std::move(tree);
      return left_split;
    } else {
      auto right_split = SplitTree(std::move(tree->right),
                                   n - TreeSize(tree->left) - 1);
      tree->right = std::move(right_split.first);
      UpdateTree(*tree);
      right_split.first = std::move(tree);
      return right_split;
    }
  }

  T SetValue(Node &tree, unsigned position, T value) {
    PushReverse(tree);

    T old;
    if (TreeSize(tree.left) > position) {
      old = SetValue(*tree.left, position, value);
    } else if (TreeSize(tree.left) == position) {
      old = tree.value;
      tree.value = value;
    } else {
      old = SetValue(*tree.right, position - TreeSize(tree.left) - 1, value);
    }

    UpdateTree(tree);
    return old;
  }

  // Node parametrs access helpers
  int TreeSize(const NodePtr &tree) {
    return tree ? tree->size : 0;
  }
  int TreeSum(const NodePtr &tree) {
    return tree ? tree->sum : 0;
  }

  void InOrderDump(Node &tree, std::vector<T> &values) {
    PushReverse(tree);

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

  // Move reversed state from node to it's children to do the actual reverse
  // (on one level)
  void PushReverse(Node &tree) {
    if (tree.reversed) {
      std::swap(tree.left, tree.right);
      tree.reversed = false;
      if (tree.left)
        ReverseTree(*tree.left);
      if (tree.right)
        ReverseTree(*tree.right);
    }
  }

  NodePtr TreeNextPermutation(NodePtr tree) {
    auto suffix_split = SplitTree(std::move(tree), tree->suffix.begin);
    NodePtr &suffix = suffix_split.second;
    unsigned element = TreeSize(suffix_split.first) - 1;
    auto element_split = SplitTree(std::move(suffix_split.first), element);
    NodePtr &node = element_split.second;

    ReverseTree(*suffix_split.second);
    node->value = SetValue(*suffix_split.second,
                          FindBiggerThan(*suffix, node->value), node->value);
    UpdateTree(*node);

    return MergeTrees(std::move(element_split.first),
                      MergeTrees(std::move(node), std::move(suffix)));
  }

  // Used for NextPermutation; supposes, that values are sorted.
  unsigned FindBiggerThan(Node &tree, T value) {
    PushReverse(tree);

    if (tree.value > value) {
      return tree.left ? FindBiggerThan(*tree.left, value) : 0;
    } else if (tree.right) {
      return TreeSize(tree.left) + 1 + FindBiggerThan(*tree.right, value);
    } else {
      return 1;
    }
    return 0;
  }
};

#endif // NEXT_PERMUTATION_H_
