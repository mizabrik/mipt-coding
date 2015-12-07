// Copyright Alexander Vasilyev

#ifndef SEGMENTS_ARRAY_H_
#define SEGMENTS_ARRAY_H_

#include <cassert>
#include <cstring>

#include <type_traits>

const int kArraySpace = 10;

template<typename T>
class Array {
 public:
  static_assert(std::is_pod<T>::value, "Only PODs are supported");

  explicit Array(int initial_length) {
    assert(initial_length >= 0);

    size_ = initial_length + kArraySpace;
    length_ = initial_length;

    array_ = new T[size_];
    memset(array_, 0, sizeof(T) * size_);
  }

  Array() : Array(0) {}

  Array(const Array<T> &source) {
    size_ = source.size_;
    length_ = source.length_;

    array_ = new T[size_];
    memcpy(array_, source.array_, sizeof(T) * size_);
  }

  ~Array() {
    delete[] array_;
  }

  const Array<T>& operator=(const Array<T> &source) {
    if (source == *this)
      return *this;

    size_ = source.size_;
    length_ = source.length_;

    delete[] array_;

    array_ = new T[size_];
    memcpy(array_, source.array_, sizeof(T) * size_);

    return *this;
  }

  bool operator==(const Array<T> &source) const {
    return array_ == source.array_;
  }

  T & operator[](int index) {
    assert(index >= 0);

    if (index >= length_) {
      if (index >= size_) {
        size_ = index + kArraySpace + 1;

        T *new_array = new T[size_];
        memcpy(new_array, array_, sizeof(T) * length_);
        memset(new_array + length_, 0, sizeof(T) * (size_ - length_));

        delete[] array_;
        array_ = new_array;
      }

      length_ = index + 1;
    }

    return array_[index];
  }

  const T& operator[](int index) const {
    assert(index >= 0 && index < length_);

    return array_[index];
  }


  int length() const {
    return length_;
  }

  T Push(T element) {
    (*this)[this->length()] = element;

    return element;
  }

  Array<T> Slice(int begin, int end) {
    assert(begin >= 0 && begin < length());
    assert(end >= begin && end < length());

    int length = end - begin;

    Array<T> slice(length);
    memcpy(slice.array_, array_ + begin, sizeof(T) * length);

    return slice;
  }

  void Sort() {
    MergeSort(0, length());
  }

 private:
  void MergeSort(int begin, int end) {
    if (begin + 1 == end)
      return;

    int division = begin + (end - begin) / 2;

    MergeSort(begin, division);
    MergeSort(division, end);
    
    Array<T> left_part = Slice(begin, division);
    int left_end = left_part.length();

    int left = 0;
    int right = division;

    for (int i = begin; left < left_end; ++i) {
      if (right < end && left_part[left] > (*this)[right]) {
        (*this)[i] = (*this)[right];
        ++right;
      } else {
        (*this)[i] = left_part[left];
        ++left;
      }
    }
  }

  T *array_;
  int length_;
  int size_;
};
#endif // SEGMENTS_ARRAY_H_
