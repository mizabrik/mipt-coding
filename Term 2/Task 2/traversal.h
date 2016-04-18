#ifndef TRAVERSAL_H_
#define TRAVERSAL_H_

#include <functional>

#include "graph.h"

using callback = std::function<void(Graph::Vertex)>;


void TraverseDfs(const Graph &graph, callback on_enter, callback on_leave);

void TraverseBfs(const Graph &graph, callback on_enter, callback on_leave);

#endif // TRAVERSAL_H_
