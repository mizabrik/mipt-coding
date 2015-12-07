// Copyright Alexander Vasilyev

#ifndef DEQUE_H_
#define DEQUE_H_

#include <cassert>
#include <cstring>

#include <type_traits>
#include <iterator>

// Automatically reallocated data with constant
// push and pop
template<typename T>
class Deque {
 public:
  static_assert(std::is_pod<T>::value, "Only PODs are supported");

  // Creates Deque with place allocated for initial_capacity elements
  explicit Deque(size_t initial_capacity=4)
    : capacity_(GetMinimalCapacity(initial_capacity)),
      size_(0),
      front_(0),
      back_(0),
      data_(new T[capacity_]) {
    std::memset(data_, 0, Capacity() * sizeof(T));
  }

  Deque(const Deque<T> &source) 
    : capacity_(source.capacity_),
      size_(source.size_),
      front_(source.front_),
      back_(source.back_),
      data_(new T[capacity_]) {
    std::memcpy(data_, source.data_, Capacity() * sizeof(T));
  }

  ~Deque() {
    delete[] data_;
  }

  const Deque<T>& operator=(const Deque<T> &source) {
    if (source == *this)
      return *this;

    capacity_ = source.capacity_;
    size_ = source.size_;
    front_ = source.front_;
    back_ = source.back_;

    delete[] data_;

    data_ = new T[capacity_];
    std::memcpy(data_, source.data_,  Capacity() * sizeof(T));

    return *this;
  }

  // Get reference to element by index
  const T& operator[](size_t index) const {
    index += front_;
    if (index > Size())
      index -= Size();

    return data_[index];
  }

  T & operator[](size_t index) {
    return const_cast<T &>(static_cast<const Deque<T> *>(this)->operator[](index));
  }

  // Number of elements deque is capable of storing without reallocation
  size_t Capacity() const {
    return capacity_;
  }

  // Number of elements in deque
  size_t Size() const {
    return size_;
  }

  // Returns true if there are no elements in deque
  bool Empty() const {
    return Size() == 0;
  }

  // Append element before front
  void PushFront(const T &element) {
    GrowIfNecessary();

    if (front_ == 0)
      front_ = Capacity();
    --front_;

    data_[front_] = element;
    ++size_;
  }

  // Returns front element and erases it
  T PopFront() {
    T element = data_[front_];

    ++front_;
    if (front_ == Capacity())
      front_ = 0;

    --size_;

    ShrinkIfNecessary();

    return element;
  }

  // Add element after current last element
  void PushBack(const T &element) {
    GrowIfNecessary();

    data_[back_] = element;

    ++back_;
    if (back_ == Capacity()) {
      back_ = 0;
    }
    ++size_;
  }

  // Returns last element and erases it
  T PopBack() {
    assert(Size() > 0);

    if (back_ == 0)
      back_ = Capacity();
    --back_;
    --size_;

    T element = data_[back_];

    ShrinkIfNecessary();

    return element;
  }

  // Constant reference to first element
  const T & Front() const {
    return (*this)[0];
  }

  // Reference to first element
  T & Front() {
    return const_cast<T &>(static_cast<const Deque<T> *>(this)->Front());
  }

  // Constant reference to last element
  const T & Back() const {
    return (*this)[Size() ? Size() - 1 : 0];
  }

  // Reference to last element
  T & Back() {
    return const_cast<T &>(static_cast<const Deque<T> *>(this)->Back());
  }

  // Iterator for deque; no range checks are done
  // PushFront: invalidates all if reallocation is done
  // PopFront: invalidates removed or all if reallocation is done
  // PushBack: invalidates end() or all if reallocation is done
  // PopBack: invalidates removed element and end() or all if reallocation is done
  template <bool constant = false>
  class DequeIterator : public std::iterator<std::random_access_iterator_tag,
      typename std::conditional<constant, const T, T>::type> {
    friend Deque<T>;

   public:
    using This = DequeIterator<constant>;
    using pointer = typename std::conditional<constant, const T *, T *>::type;
    using reference = typename std::conditional<constant, const T &, T &>::type;

    // Allow conversion from non-constant to constant iterator
    DequeIterator(const DequeIterator<false> &other)
        : index_(other.index_),
          deque_(other.deque_) {}

    reference operator *() {
      return deque_[index_];
    }

    This & operator [](size_t offset) {
      return *(*this + offset);
    }

    bool operator ==(const This &other) {
      return &deque_ == &other.deque_ && index_ == other.index_;
    }

    bool operator !=(const This other) {
      return index_ != other.index_;
    }

    bool operator <(const This &other) {
      return &deque_ == &other.deque_ && index_ < other.index_;
    }

    bool operator <=(const This &other) {
      return *this == other || *this < other;
    }

    bool operator >(const This &other) {
      return &deque_ == &other.deque_ && index_ > other.index_;
    }

    bool operator >=(const This &other) {
      return *this == other || *this > other;
    }

    This & operator +=(const long add) {
      index_ += add;
      return *this;
    }

    This operator +(const long add) {
      This copy(*this);
      copy += add;

      return copy;
    }

    This & operator -=(const long sub) {
      return *this += (-sub);
    }

    This operator -(const long sub) {
      return *this + (-sub);
    }

    This & operator ++() {
      *this += 1;

      return *this;
    }

    This & operator ++(int) {
      This copy(*this);
      ++*this;

      return copy;
    }

    This operator --() {
      *this -= 1;

      return *this;
    }

    This operator --(int) {
      This copy(*this);
      --*this;

      return copy;
    }

    long operator -(const This &other) {
      return index_ - other.index_;
    }

   private:
    using DequeT = typename std::conditional<constant, const Deque<T>, Deque<T>>::type;
    
    DequeIterator(DequeT &deque, const long index)
        : index_(index),
          deque_(deque) {}

    long index_;
    DequeT &deque_;
  };

  using Iterator = DequeIterator<false>;
  using ConstantIterator = DequeIterator<true>;
  using ReverseIterator = std::reverse_iterator<Iterator>;
  using ConstantReverseIterator = std::reverse_iterator<ConstantIterator>;

  ConstantIterator cbegin() const {
    return ConstantIterator(*this, 0);
  }

  Iterator begin() {
    return Iterator(*this, 0);
  }

  ConstantIterator begin() const {
    return cbegin();
  }

  ConstantIterator cend() const {
    return ConstantIterator(*this, Size());
  }

  Iterator end() {
    return Iterator(*this, Size());
  }

  ConstantIterator end() const {
    return cend();
  }

  ConstantReverseIterator crbegin() const {
    return ConstantReverseIterator(cend());
  }

  ReverseIterator rbegin() {
    return ReverseIterator(end());
  }

  ConstantIterator rbegin() const {
    return crbegin();
  }

  ConstantReverseIterator crend() const {
    return ConstantReverseIterator(cbegin());
  }

  ReverseIterator rend() {
    return ReverseIterator(begin());
  }

  ConstantIterator rend() const {
    return crend();
  }

 private:
  // Uses bit-wise magic to get minimal power of 2 greater than desired capacity
  size_t GetMinimalCapacity(size_t desired) {
    size_t capacity = 2;
    while (desired & ~(capacity - 1))
      capacity <<= 1;

    return capacity;
  }

  // Grow if no more elements could be placed
  void GrowIfNecessary() {
    if (Size() + 1 == Capacity())
      Realloc(capacity_ * 2);
  }

  // Shrink to fit memory upper bound
  void ShrinkIfNecessary() {
    if (Size() && Size() * 4 < Capacity())
      Realloc(capacity_ / 4);
  }

  // Reallocate deque with new size; locates front and back correctly
  void Realloc(size_t capacity) {
    assert(capacity >= Size());

    T *new_data = new T[capacity];
    if (front_ < back_) {
      std::memcpy(new_data, data_ + front_, Size() * sizeof(T));
    } else {
      const size_t left_part_size = back_;
      const size_t right_part_size = Size() - left_part_size;
      std::memcpy(new_data + right_part_size, data_, left_part_size * sizeof(T));
      std::memcpy(new_data, data_ + front_, right_part_size * sizeof(T));
    }
    std::memset(new_data + Size(), 0, (capacity - Size()) * sizeof(T));

    front_ = 0;
    back_ = Size();

    delete[] data_;
    data_ = new_data;

    capacity_ = capacity;
  }

  size_t capacity_;
  size_t size_;
  size_t front_;
  size_t back_;
  T *data_;
};

#endif // ARRAY_H_
