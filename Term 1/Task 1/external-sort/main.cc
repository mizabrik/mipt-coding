#include <iostream>
#include <fstream>

#include "external_sort.h"
#include "util.h"
#include "file_mapper.h"

using type = char;

int main(int argc, char **argv) {
  if (argc < 4) {
    std::cout << "Usage: " << argv[0] << " FILE DEST CHUNK" << std::endl;
    
    return 0;
  }

  FileMapper<type> file(argv[1]);

  const size_t chunk_memory = ParseSize(argv[3]);
  const size_t chunk_size = chunk_memory / sizeof(type);

  FileMapper<type> temp("temp", std::ios::trunc);
  std::ofstream destination(argv[2]);

  external_sort(&file, &temp, &destination, chunk_size);

  return 0;
}
