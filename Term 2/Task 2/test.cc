#include <cstdlib>
#include <ctime>

#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "graph.h"
#include "traversal.h"

Graph GenerateGraph(Graph::Vertex size, unsigned seed = 42) {
  std::set<Graph::Edge> edges;
  std::srand(seed);

  for (Graph::Vertex vertex = 0; vertex < size; ++vertex) {
    Graph::Vertex max_degree = std::rand() % (size + 1);

    for (Graph::Vertex i = 0; i < max_degree; ++i) {
      Graph::Vertex neighbour = std::rand() % size;
      edges.emplace(vertex, neighbour);
    }
  }

  return Graph(size, edges);
}

bool TestTraverse(const Graph &graph, Traverse traverse) {
  std::vector<unsigned> enter(graph.GetSize(), 0);
  std::vector<unsigned> leave(graph.GetSize(), 0);

  auto on_enter = [&enter](Graph::Vertex vertex) { ++enter[vertex]; };
  auto on_leave = [&leave](Graph::Vertex vertex) { ++leave[vertex]; };

  traverse(graph, on_enter, on_leave);

  for (Graph::Vertex vertex = 0; vertex < graph.GetSize(); ++vertex)
    if (enter[vertex] != 1 || leave[vertex] != 1)
      return false;
  return true;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cout << "Usage: " << argv[0] << " SIZE" << std::endl;
    return EXIT_FAILURE;
  }

  int size = std::stoi(argv[1]);
  if (size <= 0) {
    std::cout << "Size of graph must be a positive number" << std::endl;
    return EXIT_FAILURE;
  }

  int seed = std::time(nullptr);
  std::cout << "Testing seed " << seed << "..." << std::endl;
  auto graph = GenerateGraph(size, seed);

  std::cout << "DFS: ";
  if (TestTraverse(graph, TraverseDfs)) {
    std::cout << "OK";
  } else {
    std::cout << "failed";
  }
  std::cout << std::endl;

  std::cout << "BFS: ";
  if (TestTraverse(graph, TraverseBfs)) {
    std::cout << "OK";
  } else {
    std::cout << "failed";
  }
  std::cout << std::endl;

  return EXIT_SUCCESS;
}
