#ifndef HEAP_H_
#define HEAP_H_

template <typename T>
class HeapInterface {
 public:
  virtual ~HeapInterface() {};

  // Add key to heap
  virtual void Insert(T &&key) = 0;

  // Retrun minimal key, stored in heap
  virtual const T & GetMinimum() = 0;

  // Retrun minimal key, stored in heap and remove it from heap
  virtual T && ExtractMinimum() = 0;

  // Try to embed heap other into this heap (cleras heap other).
  // Return value indicates if meld was successful
  // (could fail if heaps are incompatible, both remain unmodified then)
  virtual void Meld(HeapInterface *other) = 0;

  virtual std::size_t size() = 0;
};

#endif // HEAP_H_
