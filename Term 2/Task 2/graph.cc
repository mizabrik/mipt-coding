#include "graph.h"

#include <set>
#include <utility>

Graph::Graph(Vertex size, std::set<Edge> edges) : neighbours_(size) {
  for (auto edge : edges) {
    neighbours_[edge.first].insert(edge.second);
  }
}

const std::set<Graph::Vertex> &Graph::GetNeighbours(Vertex vertex) const {
  assert(vertex < GetSize());
  return neighbours_[vertex];
}

Graph::Vertex Graph::GetSize() const {
  return neighbours_.size();
}
