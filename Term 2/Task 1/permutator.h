#ifndef PERMUTATOR_H_
#define PERMUTATOR_H_

#include <cstdlib>
#include <cassert>

#include <vector>
#include <memory>
#include <type_traits>

// Data type, which supports inserting, assigning values into array
// and RSQ and next_permutation on range
template<typename T>
class PermutatorInterface {
  static_assert(std::is_arithmetic<T>::value,
                "Only arithmetic values are supported");
 public:
  PermutatorInterface() = default;

  PermutatorInterface(const PermutatorInterface &other) = delete;

  PermutatorInterface operator =(const PermutatorInterface &other) = delete;

  // Insert value at given position
  virtual void Insert(unsigned position, T value);

  // Get array size
  virtual unsigned Size();

  // Assign value at given position
  virtual void Set(unsigned position, T value);

  // Change interval [begin, end) to it's next permutation
  virtual bool NextPermutation(unsigned begin, unsigned end);

  // Dump current array values into vector
  virtual std::vector<T> Dump();

  // Get RSQ on [begin, end)
  virtual T GetSum(unsigned begin, unsigned end);

  virtual ~PermutatorInterface() {};
};

#endif // PERMUTATOR_H_
