#include <cstdlib>

#include <iostream>
#include <vector>
#include <algorithm>

#include "permutator.h"
#include "naive_permutator.h"
#include "treap_permutator.h"
#include "splay_permutator.h"

int kRange = 64;

enum class TestType { kNaive, kTree, kVerify };

clock_t TestTime(PermutatorInterface<int> &permutator,
                 unsigned count, unsigned seed);
clock_t Verify(PermutatorInterface<int> &a, PermutatorInterface<int> &b,
                 unsigned count, unsigned seed);

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
  std::cout.flush();
  NaivePermutator<int> naive;
  time = TestTime(naive, count, seed);
  std::cout << ((double) time / CLOCKS_PER_SEC) << std::endl;

  /*std::cout << "Testing treap realization... ";
  std::cout.flush();
  TreapPermutator<int> treap;
  time = TestTime(treap, count, seed);
  std::cout << ((double) time / CLOCKS_PER_SEC) << std::endl;*/

  std::cout << "Testing splay realization... ";
  std::cout.flush();
  SplayPermutator<int> splay;
  time = TestTime(splay, count, seed);
  std::cout << ((double) time / CLOCKS_PER_SEC) << std::endl;

  /*NaivePermutator<int> a;
  TreapPermutator<int> b;
  std::cout << "Verifying treap... ";
  std::cout.flush();
  std::cout << (Verify(a, b, count, seed) ? "OK." : "Something's wrong.")
      << std::endl;*/

  NaivePermutator<int> c;
  SplayPermutator<int> d;
  std::cout << "Verifying splay... ";
  std::cout.flush();
  std::cout << (Verify(c, d, count, seed) ? "OK." : "Something's wrong.")
      << std::endl;

  return 0;
}

clock_t TestTime(PermutatorInterface<int> &permutator,
                 unsigned count, unsigned seed) {
  enum class Command { kInsert, kSet, kPermute, kAdd, kCommandNum };

  srand(seed);
  unsigned total = 0;

  clock_t start = clock();
  while (count) {
    Command command = static_cast<Command>(rand() % static_cast<int>(Command::kCommandNum));
    unsigned position, begin, end;
    int value;

    switch (command) {
      case Command::kInsert:
        position = std::rand() % (total + 1);
        value = std::rand() % kRange;
        permutator.Insert(position, value);
        
        ++total;

        --count;

        break;

      case Command::kSet:
        if (total == 0)
          continue;
        position = std::rand() % total;
        value = std::rand() % kRange;
        permutator.Set(position, value);

        --count;

        break;

      case Command::kAdd:
        if (total == 0)
          continue;

        begin = rand() % total;
        end = begin + (rand() % (total - begin));
        value = rand() % kRange;
        permutator.Add(begin, end, value);

        --count;

      case Command::kPermute:
        if (total == 0)
          continue;

        begin = rand() % total;
        end = begin + (rand() % (total - begin));
        permutator.NextPermutation(begin, end);

        --count;

        break;
      
      default:
        continue;
    }
  }
  clock_t end = clock();

  return end - start;
}

clock_t Verify(PermutatorInterface<int> &a, PermutatorInterface<int> &b,
                 unsigned count, unsigned seed) {
  enum class Command { kInsert, kSet, kPermute, kAdd, kCommandNum };

  srand(seed);
  unsigned total = 0;

  while (count) {
    Command command = static_cast<Command>(rand() % static_cast<int>(Command::kCommandNum));
    unsigned position, begin, end;
    int value;

    switch (command) {
      case Command::kInsert:
        position = std::rand() % (total + 1);
        value = std::rand() % kRange;
        a.Insert(position, value);
        b.Insert(position, value);
        if (a.Dump() != b.Dump())
          return false;
        
        ++total;

        --count;

        break;

      case Command::kSet:
        if (total == 0)
          continue;
        position = std::rand() % total;
        value = std::rand() % kRange;
        a.Set(position, value);
        b.Set(position, value);
        if (a.Dump() != b.Dump())
          return false;

        --count;

        break;

      case Command::kPermute:
        if (total == 0)
          continue;

        begin = rand() % total;
        end = begin + (rand() % (total - begin));
        a.NextPermutation(begin, end);
        b.NextPermutation(begin, end);
        if (a.Dump() != b.Dump())
          return false;

        --count;

        break;

      case Command::kAdd:
        if (total == 0)
          continue;

        begin = rand() % total;
        end = begin + (rand() % (total - begin));
        value = rand() % kRange;
        a.Add(begin, end, value);
        b.Add(begin, end, value);
        if (a.Dump() != b.Dump())
          return false;

        --count;

        break;
      
      default:
        continue;
    }
  }

  return true;
}
