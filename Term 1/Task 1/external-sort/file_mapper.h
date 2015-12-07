#ifndef FILE_IO_H_
#define FILE_IO_H_

#include <fstream>
#include <type_traits>

// Allows random access to file as sequence of elements
template <typename T>
class FileMapper {
  static_assert(std::is_arithmetic<T>::value, "Arithmetic types are supported");

 public:
  FileMapper(std::string file_name, std::ios::openmode flags)
    : stream_(file_name, std::ios::in | std::ios::out | flags) {
    if (Size() % sizeof(T))
      throw std::invalid_argument("file is corrupted");
  }

  FileMapper(std::string file_name) : FileMapper(file_name, std::ios::openmode()) {}

  // Get number of elements in file
  size_t Count() {
    return Size() / sizeof(T);
  }

  // Read count elements starting from offset into s
  size_t Read(T *s, size_t count, size_t offset) {
    stream_.clear();
    stream_.seekg(offset * sizeof(T), std::ios::beg);
    stream_.read((char *) s, count * sizeof(T));
    return stream_.gcount() / sizeof(T);
  }

  // Write count elements from s into file starting from offset
  void Write(T *s, size_t count, size_t offset) {
    stream_.clear();
    stream_.seekp(offset * sizeof(T), std::ios::beg);
    stream_.write((char *) s, count * sizeof(T));
    stream_.flush();
  }

 private:
  // Get file size in bytes
  size_t Size() {
    stream_.seekg(0, std::ios::end);
    return stream_.tellg();
  }

  std::fstream stream_;
};

#endif // FILE_IO_H_
