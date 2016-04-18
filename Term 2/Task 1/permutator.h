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
  // Insert value at given position
  virtual void Insert(unsigned position, T value) = 0;

  // Get array size
  virtual unsigned Size() = 0;

  // Assign value at given position
  virtual void Set(unsigned position, T value) = 0;

  // Change interval [begin, end) to it's next permutation
  virtual bool NextPermutation(unsigned begin, unsigned end) = 0;

  // Dump current array values into vector
  virtual std::vector<T> Dump() = 0;

  // Get RSQ on [begin, end)
  virtual T GetSum(unsigned begin, unsigned end) = 0;
 
  // Add value to every element in interval [begin, end) 
  virtual void Add(unsigned begin, unsigned end, T value) = 0;

  virtual ~PermutatorInterface() {};
};

#endif // PERMUTATOR_H_
