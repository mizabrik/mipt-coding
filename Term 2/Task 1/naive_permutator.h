#ifndef NAIVE_PERMUTATOR_H_
#define NAIVE_PERMUTATOR_H_

#include "permutator.h"

#include <vector>
#include <algorithm>

// Realization with std::vector and std::next_permutation
template<typename T>
class NaivePermutator : public PermutatorInterface<T> {
 public:
  virtual void Insert(unsigned position, T value) {
    data_.insert(data_.begin() + position, value);
  }

  virtual unsigned Size() {
    return data_.size();
  }

  virtual void Set(unsigned position, T value) {
    data_[position] = value;
  }

  virtual bool NextPermutation(unsigned begin, unsigned end) {
    return std::next_permutation(data_.begin() + begin, data_.begin() + end);
  }

  virtual std::vector<T> Dump() {
    return data_;
  }

  virtual T GetSum(unsigned begin, unsigned end) {
    T sum = 0;
    for (auto it = data_.begin() + begin; it != data_.begin() + end; ++it)
      sum += *it;

    return sum;
  }
  
  void Add(unsigned begin, unsigned end, T value) {
    for (auto it = data_.begin() + begin; it != data_.begin() + end; ++it)
      *it += value;
  }

 private:
  std::vector<T> data_;
};

#endif // NAIVE_PERMUTATOR_H_
