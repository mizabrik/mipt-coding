#ifndef SEGMENT_H_
#define SEGMENT_H_

#include <vector>

class SegmentTree {
 public:
  SegmentTree(std::vector<int> &array);

  int GetMin(std::size_t begin, std::size_t end);

  void UpdateSegment(std::size_t begin, std::size_t end, int value);

 private:
  struct Value {
    Value(int value);

    int value;
    bool updated;
  };

  class Node {
   public:
    Node(std::vector<Value> &data, std::size_t id,
                        std::size_t begin, std::size_t end);

    Node left();
    Node right();

    std::size_t begin();
    std::size_t end();
    std::size_t middle();

    int Min(std::size_t begin, std::size_t end);
    void Update(std::size_t begin, std::size_t end, int value);

    void Push();

    int & value();

   private:
    bool updated() {
      return data_[id_].updated;
    }

    std::vector<Value> &data_;
    std::size_t id_;

    std::size_t begin_;
    std::size_t end_;
  };

  static const int kNeutral;

  void BuildTree(std::vector<int> &array, Node tree);

  std::vector<Value> data_;
  Node root_;
};

#endif // SEGMENT_H_
