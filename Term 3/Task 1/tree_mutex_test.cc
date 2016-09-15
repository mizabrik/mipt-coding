#include <iostream>
#include "tree_mutex.h"
#include <vector>
#include <thread>

int main() {
  unsigned n_threads = std::thread::hardware_concurrency();
  std::vector<std::thread> threads;
  long counter = 0;
  const unsigned count = 1000000;
  tree_mutex mutex(n_threads);

  std::cout << "Starting " << n_threads << " threads, awaiting " << (count * n_threads) << std::endl;

  for (unsigned id = 0; id < n_threads; ++id) {
    threads.emplace_back([&counter, count, &mutex, id] {
      for (unsigned i = 0; i < count; ++i) {
        mutex.lock(id);
        ++counter;
        mutex.unlock(id);
      }
    });
  }

  for (auto &thread : threads)
    thread.join();

  std::cout << "Result: " << counter << std::endl;

  return 0;
}
