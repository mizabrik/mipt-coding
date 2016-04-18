#ifndef TRAVERSAL_H_
#define TRAVERSAL_H_

#include <functional>

#include "graph.h"

using Callback = std::function<void(Graph::Vertex)>;
using Traverse = std::function<void(const Graph &, Callback, Callback)>;


void TraverseDfs(const Graph &graph, Callback on_enter, Callback on_leave);

void TraverseBfs(const Graph &graph, Callback on_enter, Callback on_leave);

#endif // TRAVERSAL_H_
