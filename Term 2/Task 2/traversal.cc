#include "traversal.h"

#include <functional>
#include <queue>
#include <vector>

#include "graph.h"

void TraverseDfs(const Graph &graph, Callback on_enter, Callback on_leave) {
  std::vector<bool> visited(graph.GetSize(), false);

  std::function<void(Graph::Vertex)> Traverse = [&](Graph::Vertex vertex) {
    visited[vertex] = true;
    on_enter(vertex);

    for (auto neighbour : graph.GetNeighbours(vertex))
      if (!visited[neighbour])
        Traverse(neighbour);
    
    on_leave(vertex);
  };

  for (Graph::Vertex vertex = 0; vertex < graph.GetSize(); ++vertex)
    if (!visited[vertex])
      Traverse(vertex);
}

void TraverseBfs(const Graph &graph, Callback on_enter, Callback on_leave) {
  std::vector<bool> visited(graph.GetSize(), false);

  std::function<void(Graph::Vertex)> Traverse = [&](Graph::Vertex vertex) {
    std::queue<Graph::Vertex> queue;
    queue.push(vertex);
    visited[vertex] = true;

    while (!queue.empty()) {
      Graph::Vertex vertex = queue.front();
      queue.pop();
      on_enter(vertex);

      for (auto neighbour : graph.GetNeighbours(vertex)) {
        if (!visited[neighbour]) {
          queue.push(neighbour);
          visited[neighbour] = true;
        }
      }

      on_leave(vertex);
    }
  };

  for (Graph::Vertex v = 0; v < graph.GetSize(); ++v)
    if (!visited[v])
      Traverse(v);
}
