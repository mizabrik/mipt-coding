// Copyright Alexander Vasilyev

#ifndef SEGMENTS_ARRAY_H_
#define SEGMENTS_ARRAY_H_

#include <cassert>
#include <cstring>

#include <type_traits>

// Automatically reallocated array with constant (
// // Remove element from the endh
// push and pop
template<typename T>
class Vector {
 public:
  static_assert(std::is_pod<T>::value, "Only PODs are supported");

  explicit Vector(size_t initial_size=0)
    : size_(initial_size),
      capacity_(GetMinimalCapacity(initial_size)) {
    array_ = new T[capacity_];
    std::memset(array_, 0, sizeof(T) * capacity_);
  }

  Vector(const Vector<T> &source) {
    capacity_ = source.capacity_;
    size_ = source.size_;

    array_ = new T[capacity_];
    std::memcpy(array_, source.array_, sizeof(T) * capacity_);
  }

  ~Vector() {
    delete[] array_;
  }

  const Vector<T>& operator=(const Vector<T> &source) {
    if (source == *this)
      return *this;

    capacity_ = source.capacity_;
    size_ = source.size_;

    delete[] array_;

    array_ = new T[capacity_];
    std::memcpy(array_, source.array_, sizeof(T) * capacity_);

    return *this;
  }

  T & operator[](size_t index) {
    assert(index >= 0 && index < size_);

    return array_[index];
  }

  const T& operator[](int index) const {
    assert(index >= 0 && index < size_);

    return array_[index];
  }


  // Number of elements in vector
  size_t size() const {
    return size_;
  }

  // Number of elements vector is capable of storing without reallocation
  size_t capacity() const {
    return capacity_;
  }

  // Append element at the end
  void PushBack(T element) {
    if (size() == capacity())
      Grow();

    ++size_;
    (*this)[size() - 1] = element;
  }

  // Remove element from the end
  T PopBack() {
    assert(size() > 0);

    T element = (*this)[size() - 1];

    --size_;
    if (size() && size() * 4 < capacity())
      Shrink();

    return element;
  }

 private:
  // Uses bit-wise magic to get minimal power of 2 greater than desired capacity
  size_t GetMinimalCapacity(size_t desired) {
    size_t capacity = 2;
    while (desired & ~(capacity - 1))
      capacity <<= 1;

    return capacity;
  }

  void Grow() {
    Realloc(capacity_ * 2);
  }

  void Shrink() {
    Realloc(capacity_ / 4);
  }

  void Realloc(size_t capacity) {
    assert(capacity >= size_);

    T *new_array = new T[capacity];
    std::memcpy(new_array, array_, sizeof(T) * size_);
    std::memset(new_array + size_, 0, sizeof(T) * (capacity - size_));

    delete[] array_;
    array_ = new_array;

    capacity_ = capacity;
  }

  T *array_;
  size_t size_;
  size_t capacity_;
};

#endif // SEGMENTS_ARRAY_H_
