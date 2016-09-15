#include <atomic>
#include <array>
#include <thread>
#include <vector>

// Mutex for two threads, every thread has to now it's id
class peterson_mutex {
 public:
  peterson_mutex();

  peterson_mutex(const peterson_mutex &other) = delete;
  peterson_mutex & operator =(const peterson_mutex &other) = delete;

  void lock(unsigned id);

  void unlock(unsigned id);

 private:
  std::atomic_uint victim_;
  std::array<std::atomic_bool, 2> want_;

  unsigned other(unsigned id);
};

// Mutex for n threads, every thread has to now it's id
class tree_mutex {
 public:
  tree_mutex(unsigned n_threads);
  
  tree_mutex(const tree_mutex &) = delete;
  tree_mutex & operator =(const tree_mutex &) = delete;

  void lock(unsigned id);

  void unlock(unsigned id);

 private:
  static unsigned tree_size(unsigned n_threads);
  static unsigned get_bit(unsigned n, unsigned mask);

  std::vector<peterson_mutex> mutexes_;
};

peterson_mutex::peterson_mutex() {
  victim_.store(0);
  want_[0].store(false);
  want_[1].store(false);
}

void peterson_mutex::lock(unsigned id) {
  want_[id].store(true);
  victim_.store(id);

  while (want_[other(id)].load() && victim_.load() == id)
    std::this_thread::yield();
}

void peterson_mutex::unlock(unsigned id) {
  want_[id].store(false);
}

unsigned peterson_mutex::other(unsigned id) {
  return 1 - id;
}

tree_mutex::tree_mutex(unsigned n_threads) : mutexes_(tree_size(n_threads)) {}

void tree_mutex::lock(unsigned id) {
  unsigned mutex_id = mutexes_.size() + id;

  do {
    unsigned local_id = 1 - mutex_id % 2;
    mutex_id = (mutex_id - 1) / 2;
    mutexes_[mutex_id].lock(local_id);
  } while (mutex_id > 0);
}

void tree_mutex::unlock(unsigned id) {
  // tree path is restored by bits of thread number
  unsigned mask = (mutexes_.size() + 1) / 2; // higher bit of thread id
  unsigned mutex_id = 0;

  while (mask) {
    mutexes_[mutex_id].unlock(get_bit(id, mask));
    mutex_id = mutex_id * 2 + 1 + get_bit(id, mask);
    mask /= 2;
  }
}

unsigned tree_mutex::tree_size(unsigned n_threads) {
  unsigned size = 2;

  while (size < n_threads)
    size *= 2;

  return size - 1;
}

unsigned tree_mutex::get_bit(unsigned n, unsigned mask) {
  return n & mask ? 1 : 0;
}
