#ifndef SORT_H_
#define SORT_H_

#include <cassert>

#include <vector>
#include <algorithm>
#include <stdexcept>
#include <fstream>

#include "file_mapper.h"
#include "chunk.h"

// Put sorted content of file into destination;
// temp is used for sorted chunks storing.
template <typename T>
void external_sort(FileMapper<T> *file, FileMapper<T> *temp,
                   std::ofstream *destination, size_t chunk_size) {
  assert(chunk_size != 0);

  const size_t total_count = file->Count();
  const size_t chunks_num = total_count / chunk_size + (total_count % chunk_size ? 1 : 0);
  const size_t portion = chunk_size >= chunks_num
                              ? chunk_size / chunks_num
                              : 1;

  std::vector<SortingChunk<T>> chunks;
  { 
    for(size_t offset = 0; offset < total_count; offset += chunk_size)
      chunks.push_back(SortingChunk<T>(file, temp, chunk_size, offset, portion));
  }

  using min = std::pair<T, size_t>;

  std::vector<min> heap;
  for (size_t i = 0; i < chunks.size(); ++i)
    heap.push_back(min(chunks[i].Get(), i));
  std::make_heap(heap.begin(), heap.end(), std::greater<min>());

  T *merged_chunk = new T[chunk_size];

  while (heap.size() > 0) {
    for(size_t count = 0; count < chunk_size && heap.size(); ++count) {
      size_t chunk = heap[0].second;

      if (chunks[chunk].ElementsLeft())
          heap.push_back(min{chunks[chunk].Get(), chunk});
      std::pop_heap(heap.begin(), heap.end(), std::greater<min>());

      merged_chunk[count] = heap[heap.size() - 1].first;
      heap.pop_back();
    }

    destination->write((char *) merged_chunk, sizeof(T) * chunk_size);
  }

  delete []merged_chunk;
}

#endif // SORT_H_
