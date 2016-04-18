#ifndef GRAPH_H_
#define GRAPH_H_

#include <cassert>
#include <cstdint>

#include <set>
#include <utility>
#include <vector>

class Graph {
 public:
  using Vertex = uintmax_t;
  using Edge = std::pair<Vertex, Vertex>;

  // Create new graph with `size` vertexes from set of edges
  Graph(Vertex size, std::set<Edge> edges);

  const std::set<Vertex> &GetNeighbours(Vertex vertex) const;

  Vertex GetSize() const;

 private:
  std::vector<std::set<Vertex>> neighbours_;
};

#endif // GRAPH_H_
