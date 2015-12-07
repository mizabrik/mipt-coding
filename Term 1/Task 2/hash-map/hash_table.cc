#include "hash_table.h"

#include <string>
#include <vector>
#include <iterator>

const std::string HashTable::kDeleted;

HashTable::HashTable() : count_(0), table_(kInitialSize, nullptr) {}

HashTable::~HashTable() {
  for (auto key : table_) {
    if (key != &kDeleted)
      delete key;
  }
}

bool HashTable::Has(const std::string &key) const {
  for (size_t probe : Probes(table_, key)) {
    if (table_[probe] == nullptr) {
      break;
    } else if (table_[probe] != &kDeleted && *table_[probe] == key) {
      return true;
    }
  }

  return false;
}

bool HashTable::Add(const std::string &key) {
  // Rehash if 
  if (count_ * 4 >= Size() * 3)
    Grow();

  size_t empty_probe = Size();
  for (size_t probe : Probes(table_, key)) {
    if (table_[probe] == nullptr) {
      if (empty_probe == Size())
        empty_probe = probe;
      break;
    } else if (table_[probe] == &kDeleted) {
      if (empty_probe == Size())
        empty_probe = probe;
    } else if (*table_[probe] == key) {
      return false;
    }
  }

  if (empty_probe != Size()) {
    table_[empty_probe] = new std::string(key);
    ++count_;
    return true;
  }

  return false;
}

bool HashTable::Remove(const std::string &key) {
  for (auto probe : Probes(table_, key)) {
    if (table_[probe] == nullptr) {
      break;
    } else if (table_[probe] != &kDeleted && *table_[probe] == key) {
      delete table_[probe];
      table_[probe] = &kDeleted;
      --count_;
      return true;
    }
  }
  return false;
}

size_t HashTable::Size() const {
  return table_.size();
}

void HashTable::Grow() {
  std::vector<std::string const *> old_table(2 * Size(), nullptr);
  std::swap(table_, old_table);
  count_ = 0;

  for (auto key : old_table)
    if (key != nullptr && key != &kDeleted) {
      Add(*key);
      delete key;
    }
}

HashTable::Probes::Probes(const std::vector<std::string const *> &table, const std::string &key)
  : table_(table),
    hash1_(Hash(key)),
    hash2_((2 * hash1_ + 1) % table_.size()) {}

size_t HashTable::Probes::Hash(const std::string &key) const {
  size_t hash = 0;

  for (auto symbol : key)
    hash = (hash * kHashParameter + symbol) % table_.size();

  return hash;
}

HashTable::Probes::iterator HashTable::Probes::begin() {
  return iterator(*this, 0);
}

HashTable::Probes::iterator  HashTable::Probes::end() {
  return iterator(*this, table_.size());
}

template <typename T>
HashTable::Probes::ProbeIterator<T>::ProbeIterator(const Probes &probes, size_t probe)
    : probes_(probes),
      probe_(probe) {}

template <typename T>
T HashTable::Probes::ProbeIterator<T>::operator* () {
  return (probes_.hash1_ + probe_ * probes_.hash2_) % probes_.table_.size();
}

template <typename T>
HashTable::Probes::ProbeIterator<T> & HashTable::Probes::ProbeIterator<T>::operator++() {
  ++probe_;
  return *this;
}

template <typename T>
bool HashTable::Probes::ProbeIterator<T>::operator!=(ProbeIterator<T> other) {
  return probe_ != other.probe_;
}
