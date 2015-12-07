#include <cstdlib>

#include <iostream>
#include <vector>

#include "heap.h"
#include "binomial_heap.h"
#include "skew_heap.h"

template <typename Heap>
clock_t test_heap(unsigned count, unsigned seed, std::vector<int> &result);

int main() {
  unsigned seed = time(nullptr);

  std::cout << "Enter number of operations" << std::endl;
  int count = 0;
  std::cin >> count;

  std::vector<int> skew;
  std::vector<int> leftist;
  std::vector<int> binomial;

  clock_t time;

  std::cout << "Testing skew heap... ";
  time = test_heap<SkewHeap<int>>(count, seed, skew);
  std::cout << ((double) time / CLOCKS_PER_SEC) << std::endl;

  std::cout << "Testing leftist heap... ";
  time = test_heap<LeftistHeap<int>>(count, seed, leftist);
  std::cout << ((double) time / CLOCKS_PER_SEC) << std::endl;

  std::cout << "Testing binomial heap... ";
  time = test_heap<BinomialHeap<int>>(count, seed, binomial);
  std::cout << ((double) time / CLOCKS_PER_SEC) << std::endl;

  std::cout << std::endl;

  std::cout << (std::equal(skew.cbegin(), skew.cend(), leftist.cbegin())
                ? "Results of skew and leftist heaps match"
                : "Results of skew and leftist heaps mismatch") << std::endl;

  std::cout << (std::equal(leftist.cbegin(), leftist.cend(), binomial.cbegin())
                ? "Results of leftist and binomial heaps match"
                : "Results of leftist and binomial heaps mismatch") << std::endl;

  return 0;
}

template <typename Heap>
clock_t test_heap(unsigned count, unsigned seed, std::vector<int> &result) {
  enum class Command { kAddHeap, kInsert, kExtract, kMeld, kCommandNum };

  std::vector<Heap> heaps;

  srand(seed);
  unsigned total = 0;

  clock_t start = clock();
  while (count) {
    Command command = static_cast<Command>(rand() % static_cast<int>(Command::kCommandNum));
    std::size_t heap;
    std::size_t donor;
    int key;

    switch (command) {
      case Command::kAddHeap:
        heaps.emplace_back();
        heaps.back().Insert(rand());
        ++total;

        --count;
        break;

      case Command::kInsert:
        if (heaps.size() == 0)
          heaps.emplace_back();

        heaps[rand() % heaps.size()].Insert(rand());
        ++total;

        --count;

        break;

      case Command::kExtract:
        if (total == 0 || heaps.size() == 0)
          continue;

        heap = rand() % heaps.size();
        if (heaps[heap].size() == 0)
          continue;


        key = heaps[heap].ExtractMinimum();
        result.push_back(key);

        --count;

        break;

      case Command::kMeld:
        if (heaps.size() == 0)
          continue;

        heap = rand() % heaps.size();
        donor = rand() % heaps.size();
        if (heap == donor)
          continue;

        heaps[heap].Meld(&heaps[donor]);
        heaps.erase(heaps.begin() + donor);

        --count;

        break;
      
      default:
        continue;
    }
  }
  clock_t end = clock();

  return end - start;
}
