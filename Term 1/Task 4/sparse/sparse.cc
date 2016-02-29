#include "sparse.h"

#include <cmath>

#include <vector>

SparseTable::SparseTable(std::vector<int> &&data) : data_(std::move(data)) {
  PrepareLogs();
  PrepareTable();
}

int SparseTable::Query(std::size_t left, std::size_t right) {
  std::size_t log = log_[right - left];
  return data_[Compose(table_[log][left], table_[log][right - pow(2, log) + 1]).second];
}

SparseTable::Stats SparseTable::Order(std::size_t i, std::size_t j) {
  if (data_[i] > data_[j])
    std::swap(i, j);

  return std::make_pair(i, j);
}

SparseTable::Stats SparseTable::Compose(Stats a, Stats b) {
  if (data_[a.first] > data_[b.first])
    std::swap(a, b);

  return std::make_pair(a.first,
                        a.first == b.first ? MinIndex(a.second, b.second)
                                           : MinIndex(a.second, b.first));
}

std::size_t SparseTable::MinIndex(std::size_t i, std::size_t j) {
  return data_[i] < data_[j] ? i : j;
}

void SparseTable::PrepareLogs() {
  log_.push_back(0);
  for (std::size_t number = 2; number <= data_.size(); ++number) {
    log_.push_back(log_[number/2 - 1] + 1);
  }
}

void SparseTable::PrepareTable() {
  table_.emplace_back(); // Just to use real logarithm
  table_.emplace_back();
  for (std::size_t i = 0; i + 1 < data_.size(); ++i)
    table_.back().push_back(Order(i, i + 1));

  for (std::size_t power = 2, j = 4; j <= data_.size(); ++power, j *= 2) {
    table_.emplace_back();
    for (int i = 0; i + j <= data_.size(); ++i)
      table_.back().push_back(Compose(table_[power - 1][i], table_[power - 1][i + j/2]));
  }
}
