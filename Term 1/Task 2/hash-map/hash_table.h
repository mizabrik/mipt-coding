#ifndef HASH_H_
#define HASH_H_

#include <string>
#include <vector>
#include <iterator>

class HashTable {
 public:
  HashTable();

  ~HashTable();

  // Check if table contains key
  bool Has(const std::string &key) const;

  // Add key to table; if count of elements exceeds size of table,
  // rehashes the table
  bool Add(const std::string &key);

  bool Remove(const std::string &key);

  size_t Count() const;

  size_t Size() const;

 private:
  class Probes {
   public:
    Probes(const std::vector<std::string const *> &table, const std::string &key);

    template <typename T>
    class ProbeIterator : public std::iterator<std::input_iterator_tag, T> {
      friend Probes;

     public:
      T operator* ();

      ProbeIterator<T> & operator++();

      bool operator!=(ProbeIterator<T> other);
     private:
      ProbeIterator(const Probes &probes, size_t probe);
      const Probes &probes_;
      size_t probe_;
    };

    using iterator = ProbeIterator<size_t>;

    iterator begin();

    iterator end();

   private:
    size_t Hash(const std::string &key) const;

    const std::vector<std::string const *> &table_;
    const size_t hash1_;
    const size_t hash2_;

    static const int kHashParameter = 41;
  };

  void Grow();

  size_t count_;
  std::vector<std::string const *> table_;

  static const std::string kDeleted;

  // Initial size of hash table
  static const size_t kInitialSize = 16;
};

#endif // HASH_H_
