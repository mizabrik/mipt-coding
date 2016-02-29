#include <cstdlib>

#include <iostream>
#include <vector>
#include <algorithm>

#include "next_permutation.h"

int kRange = 64;

enum class TestType { kNaive, kTree, kVerify };

clock_t TestNP(unsigned count, unsigned seed, TestType);

int main() {
  unsigned seed = time(nullptr);

  std::cout << "Enter number of operations: ";
  int count = 0;
  std::cin >> count;

  std::vector<int> skew;
  std::vector<int> leftist;
  std::vector<int> binomial;

  clock_t time;

  std::cout << "Testing naive realization... ";
  time = TestNP(count, seed, TestType::kNaive);
  std::cout << ((double) time / CLOCKS_PER_SEC) << std::endl;

  std::cout << "Testing tree realization... ";
  time = TestNP(count, seed, TestType::kTree);
  std::cout << ((double) time / CLOCKS_PER_SEC) << std::endl;

  try {
    std::cout << "Verifying results... ";
    TestNP(count, seed, TestType::kVerify);
    std::cout << "OK." << std::endl;
  } catch (std::runtime_error e) {
    std::cout << e.what() << std::endl;
  }

  return 0;
}

clock_t TestNP(unsigned count, unsigned seed, TestType type) {
  enum class Command { kInsert, kSet, kPermute, kCommandNum };

  srand(seed);
  unsigned total = 0;

  NextPermutator<int> tree;
  std::vector<int> dump;

  std::vector<int> naive;

  clock_t start = clock();
  while (count) {
    Command command = static_cast<Command>(rand() % static_cast<int>(Command::kCommandNum));
    unsigned position, begin, end;
    int value;

    switch (command) {
      case Command::kInsert:
        position = std::rand() % (total + 1);
        value = std::rand() % kRange;
        switch (type) {
          case TestType::kNaive:
            naive.insert(naive.begin() + position, value);
            break;

          case TestType::kTree:
            tree.Insert(position, value);
            break;

          case TestType::kVerify:
            naive.insert(naive.begin() + position, value);
            tree.Insert(position, value);
            dump = tree.Dump();
            if (naive != dump)
              throw std::runtime_error("Mismatch on insert");
            break;
        }
        ++total;

        --count;

        break;

      case Command::kSet:
        if (total == 0)
          continue;
        position = std::rand() % total;
        value = std::rand() % kRange;
        switch (type) {
          case TestType::kNaive:
            naive[position] = value;
            break;

          case TestType::kTree:
            tree.Set(position, value);
            break;

          case TestType::kVerify:
            naive[position] = value;
            tree.Set(position, value);
            dump = tree.Dump();
            if (naive != dump)
              throw std::runtime_error("Mismatch on set");
            break;
        }

        --count;

        break;

      case Command::kPermute:
        if (total == 0)
          continue;

//        begin = rand() % total;
//        end = begin + (rand() % (total - begin));
        begin = 0;
        end = total;

        switch (type) {
          case TestType::kNaive:
            std::next_permutation(naive.begin() + begin, naive.begin() + end);
            break;

          case TestType::kTree:
            tree.NextPermutation(begin, end);
            break;

          case TestType::kVerify:
            std::next_permutation(naive.begin() + begin, naive.begin() + end);
            tree.NextPermutation(begin, end);
            dump = tree.Dump();
            if (naive != dump) {
              std::cout << std::endl;
              std::cout << begin << ' ' << end << std::endl;
              std::cout << "Naive result:" << std::endl;
              for (auto e : naive)
                std::cout << e << ' ';
              std::cout << std::endl;
              std::cout << "Tree result:" << std::endl;
              for (auto e : dump)
                std::cout << e << ' ';
              std::cout << std::endl;
              std::cout << "Got from:" << std::endl;
              std::prev_permutation(naive.begin() + begin, naive.begin() + end);
              for (auto e : naive)
                std::cout << e << ' ';
              std::cout << std::endl;
              std::cout << "Got from:" << std::endl;
              std::next_permutation(naive.begin() + begin, naive.begin() + end);
              for (auto e : naive)
                std::cout << e << ' ';
              std::cout << std::endl;
              throw std::runtime_error("Mismatch on permute");
            }
            break;
        }

        --count;

        break;
      
      default:
        continue;
    }
  }
  clock_t end = clock();

  return end - start;
}
