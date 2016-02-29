#ifndef SPARSE_H_
#define SPARSE_H_

#include <vector>

class SparseTable {
 public:
  // Create sparse table based on given values
  SparseTable(std::vector<int> &&values);

  // Get second order statistic on interval
  int Query(std::size_t left, std::size_t right);

 private:
  using Stats = std::pair<std::size_t, std::size_t>;

  // Operation definitions
  Stats Order(std::size_t i, std::size_t j);
  Stats Compose(Stats a, Stats b);
  std::size_t MinIndex(std::size_t i, std::size_t j);

  // Generates table of logarithms
  void PrepareLogs();

  // Counts stats on all intervals with lenght --- power of 2
  void PrepareTable();

  std::vector<int> data_;
  std::vector<std::size_t> log_;
  std::vector<std::vector<Stats>> table_;
};

#endif // SPARSE_H_
