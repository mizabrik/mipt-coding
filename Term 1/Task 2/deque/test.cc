#include <cstdlib>
#include <ctime>
#include <cassert>

#include <iostream>

#include "deque.h"

static const int kRunCount = 3;

// Perform n random operations on deque
void test(unsigned long n) {
  enum class Command { kPushFront, kPopFront, kPushBack, kPopBack, kAccess, kCommandNum };
  const int kRange = 2000;

  Deque<int> deque;

  srand(time(0));

  while (n) {
    Command command = static_cast<Command>(rand() % static_cast<int>(Command::kCommandNum));

    switch (command) {
      case Command::kPushBack:
        deque.PushBack(rand() % kRange);
        break;

      case Command::kPushFront:
        deque.PushFront(rand() % kRange);
        break;

      case Command::kPopBack:
        if (!deque.Size())
          continue;
        deque.PopBack();
        break;

      case Command::kPopFront:
        if (!deque.Size())
          continue;
        deque.PopFront();
        break;

      case Command::kAccess:
        if (deque.Size() < 2)
          continue;
        deque[rand() % deque.Size()] = rand() % kRange;
        break;
      default:
        continue;
    }

    --n;
  }
}

int main() {
  unsigned long n;

  std::cout << "Enter number of operation per test" << std::endl;

  while (std::cin >> n) {
    if (!n)
      std::cout << "Enter non-zero n" << std::endl;
    for (int i = 1; i <= kRunCount; ++i) {
      clock_t time = clock();
      test(n);
      time = clock() - time;
      std::cout << "Run #" << i << ": " << ((float) time / n) << "(" << time << " total)" << std::endl;
    }
  }

  return 0;
}
