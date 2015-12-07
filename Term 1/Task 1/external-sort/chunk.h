#ifndef CHUNK_H_
#define CHUNK_H_

#include <cstring>

#include <algorithm>

#include "file_mapper.h"

// Used for fetching elements of FileMapper
// in increasing order
// Elements are bufferized to reduce amount of IO operations
template <typename T>
class SortingChunk {
 public:
  SortingChunk(FileMapper<T> *file, FileMapper<T> *temp, size_t size, size_t offset, size_t buffer_size)
    : file_(temp),
      begin_(offset),
      size_(size),
      position_(begin_),
      buffer_size_(buffer_size) {
    T *data = new T[size_];
    size_ = file->Read(data, size_, begin_);
    end_ = begin_ + size_;

    std::sort(data, data + size_);
    
    file_->Write(data, size_, begin_);
    delete[] data;

    buffer_position_ = buffer_ = new T[buffer_size_];
    LoadBuffer();
  }

  SortingChunk(const SortingChunk &source)
    : file_(source.file_),
      begin_(source.begin_),
      size_(source.size_),
     position_(source.position_),
     end_(source.end_),
     buffer_size_(source.buffer_size_) {
    buffer_ = new T[buffer_size_];
    buffer_position_ = buffer_ + (source.buffer_position_- source.buffer_);
    buffer_end_ = buffer_ + (source.buffer_end_ - source.buffer_);

    ::memcpy(buffer_, source.buffer_, buffer_size_ * sizeof(T));
  }

  ~SortingChunk() {
    delete []buffer_;
  }

  // Get next element
  T Get() {
    if(position_ >= end_)
      throw 1;
    if (buffer_position_ == buffer_end_) {
      LoadBuffer();
    }
    ++position_;

    T element = *buffer_position_;
    ++buffer_position_;
    return element;
  }

  // Get amount of available elements in FileMapper
  size_t ElementsLeft() {
    return end_ - position_;
  }

 private:
  // Load next part of the buffer int memory
  size_t LoadBuffer() {
    size_t count = buffer_size_;
    if (position_ + count >= end_)
      count = ElementsLeft();

    size_t fetched = file_->Read(buffer_, count, position_);
    buffer_position_ = buffer_;
    buffer_end_ = buffer_ + fetched;

    return fetched;
  }

  FileMapper<T> *file_;
  size_t begin_;
  size_t size_;
  size_t position_;
  size_t end_;

  size_t buffer_size_;
  T *buffer_;
  T *buffer_end_;
  T *buffer_position_;
};

#endif // CHUNK_H_
