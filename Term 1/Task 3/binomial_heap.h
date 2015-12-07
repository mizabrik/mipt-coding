#ifndef BINOMIAL_HEAP_H_
#define BINOMIAL_HEAP_H_

#include <cassert>

#include <list>

#include "heap.h"

// Stores array of binomial treas satisfying heap property,
// one of each order at most
template <typename T>
class BinomialHeap : public HeapInterface<T> {
 public:
  BinomialHeap() : size_(0), minimum_(trees_.begin()) {};

  BinomialHeap(const BinomialHeap<T> &other) = delete;
  BinomialHeap<T> & operator =(const BinomialHeap<T> &other) = delete;

  BinomialHeap(BinomialHeap<T> &&other) = default;
  BinomialHeap<T> & operator =(BinomialHeap<T> &&other) = default;

  void Insert(T &&key) {
    BinomialTree new_tree(key);
    AddTree(std::move(new_tree));
    ++size_;

    UpdateMinimum();
  }

  const T & GetMinimum() {
    assert(size() != 0);

    return minimum_->key();
  }

  T && ExtractMinimum() {
    assert(size() != 0);

    BinomialTree minimal = std::move(*minimum_);
    trees_.erase(minimum_);
    --size_;

    minimal.Flattern(*this);
    UpdateMinimum();

    return minimal.ExtractKey();
  }

  void Meld(HeapInterface<T> *other_typeless) {
    BinomialHeap *other = dynamic_cast<BinomialHeap *>(other_typeless);
    assert(other != nullptr);

    size_ += other->size();
    other->size_ = 0;

    for (auto &tree : other->trees_)
      AddTree(std::move(tree));
    other->trees_.clear();

    UpdateMinimum();
  }

  std::size_t size() {
    return size_;
  }
 private:
  class BinomialTree;
  using TreeIterator = typename std::list<BinomialTree>::iterator;

  // Binomial tree of order n contains C_n^k elements on k'th level
  class BinomialTree {
   public:
    BinomialTree(const T &key)
        : key_(key),
        order_(0) {}

    BinomialTree(const BinomialTree &other) = delete;
    BinomialTree & operator =(const BinomialTree &other) = delete;

    BinomialTree(BinomialTree &&other) = default;
    BinomialTree & operator =(BinomialTree &&other) = default;

    void Append(BinomialTree &&tree) {
      assert(tree.order() == order());

      children_.emplace_front(std::move(tree));
      ++order_;
    }

    // Used to effectively extract minimal element
    void Flattern (BinomialHeap<T> &heap) {
      for (auto &subtree : children_)
        heap.AddTree(std::move(subtree));

      order_ = 0;
      children_.clear();
    }

    const T & key() {
      return key_;
    }

    T && ExtractKey() {
      return std::move(key_);
    }
     
    std::size_t order() {
      return order_;
    }

   private:
    T key_;
    std::size_t order_;
    std::list<BinomialTree> children_;
  };

  BinomialHeap(std::list<BinomialTree> &&trees) : trees_(std::move(trees)) {
    UpdateMinimum();
  }

  // Update reference to minimal element after changing trees
  void UpdateMinimum() {
    for (auto it = minimum_ = trees_.begin(); it != trees_.end(); ++it)
      if (it->key() < minimum_->key())
        minimum_ = it;
  }

  void AddTree(BinomialTree &&tree) {
    TreeIterator sibling = trees_.begin();
    while (sibling != trees_.end() && sibling->order() < tree.order())
      ++sibling;

    if (sibling == trees_.end() || sibling->order() > tree.order()) {
      trees_.insert(sibling, std::move(tree));
    } else {
      MergeTrees(*sibling, tree);
    }
  }

  // Merge two trees of one order into one
  void MergeTrees(BinomialTree &a, BinomialTree &b) {
    if (a.key() > b.key())
      std::swap(a, b);

    a.Append(std::move(b));
  }

  std::size_t size_;
  std::list<BinomialTree> trees_;
  typename std::list<BinomialTree>::iterator minimum_;
};

#endif // BINOMIAL_HEAP_H_
