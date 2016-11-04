#ifndef THREAD_SAFE_QUEUE_H_
#define THREAD_SAFE_QUEUE_H_

#include <condition_variable>
#include <deque>
#include <mutex>

template <typename T, typename Container = std::deque<T>>
class thread_safe_queue {
 public:
  explicit thread_safe_queue(size_t capacity);

  void enqueue(T v);
  void pop(T &v);

  void shutdown();

 private:
  size_t capacity_;
  volatile bool shutdown_;
  Container queue_;

  std::mutex mutex_;
  std::condition_variable capacity_cond_;
  std::condition_variable size_cond_;
};

template <typename T, typename Container>
thread_safe_queue<T, Container>::thread_safe_queue(size_t capacity)
  : capacity_(capacity),
    shutdown_(false) {}

template <typename T, typename Container>
void thread_safe_queue<T, Container>::enqueue(T v) {
  std::unique_lock<std::mutex> lock(mutex_);
  if (shutdown_) {
    throw std::runtime_error("Trying to enqueue in shut down queue");
  }

  capacity_cond_.wait(lock, [this]() {
    return shutdown_ || queue_.size() < capacity_;
  });

  if (shutdown_) {
    throw std::runtime_error("Queue was shut down during enqueue");
  }

  queue_.push_back(std::move(v));

  size_cond_.notify_one();
}

template <typename T, typename Container>
void thread_safe_queue<T, Container>::pop(T &v) {
  std::unique_lock<std::mutex> lock(mutex_);
  size_cond_.wait(lock, [this]() { return shutdown_ || !queue_.empty(); });

  if (queue_.empty()) {
    throw std::runtime_error("Popping from empty shut down queue");
  }

  v = std::move(queue_.front());
  queue_.pop_front();
  
  capacity_cond_.notify_one();
}

template <typename T, typename Container>
void thread_safe_queue<T, Container>::shutdown() {
  std::unique_lock<std::mutex> lock(mutex_);
  shutdown_ = true;
  lock.unlock();

  capacity_cond_.notify_all();
  size_cond_.notify_all();
}

#endif // THREAD_SAFE_QUEUE_H_
