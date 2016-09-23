#include <iostream>
#include <vector>
#include <queue>
#include <climits>

using std::vector;

using VVi = vector<vector<int>>;

vector<vector<int>> scan(unsigned n, unsigned m) {
  vector<vector<int>> capacity(n, vector<int>(n, 0));

  for (unsigned i = 0; i < m; ++i) {
    unsigned u, v, c;
    std::cin >> u >> v >> c;
    capacity[u - 1][v - 1] += c;
  }

  return capacity;
}

vector<int> FindPath(unsigned from, unsigned to, const VVi &capacity, VVi &flow) {
  unsigned n = capacity.size();
  vector<int> previous(n, -1);
  std::queue<unsigned> pending;
  pending.push(from);
  vector<bool> visited(n, false);
  visited[from] = true;

  while (!pending.empty()) {
    unsigned u = pending.front();
    pending.pop();
    for (unsigned v = 0; v < n; ++v) {
      if (capacity[u][v] - flow[u][v] && !visited[v]) {
        visited[v] = true;
        previous[v] = u;
        pending.push(v);
        if (v == to)
          break;
      }
    }
  }

  return previous;
}

vector<int> FindFlowPath(unsigned from, unsigned to, const VVi &capacity, VVi &flow) {
  unsigned n = capacity.size();
  vector<int> previous(n, -1);
  std::queue<unsigned> pending;
  pending.push(from);
  vector<bool> visited(n, false);
  visited[from] = true;

  while (!pending.empty()) {
    unsigned u = pending.front();
    pending.pop();
    for (unsigned v = 0; v < n; ++v) {
      if (flow[u][v] > 0 && !visited[v]) {
        visited[v] = true;
        previous[v] = u;
        pending.push(v);
        if (v == to)
          break;
      }
    }
  }

  return previous;
}

bool ImproveFlow(unsigned s, unsigned t, const VVi &capacity, VVi &flow) {
  vector<int> previous = FindPath(s, t, capacity, flow);

  if (previous[t] >= 0) {
    int curr = t;
    int prev = previous[curr];
    int min_capacity = INT_MAX;
    while (prev != -1) {
      min_capacity = std::min(min_capacity, capacity[prev][curr] - flow[prev][curr]);
      curr = prev;
      prev = previous[curr];
    }
    curr = t;
    prev = previous[curr];
    while (prev != -1) {
      flow[prev][curr] += min_capacity;
      flow[curr][prev] -= min_capacity;
      curr = prev;
      prev = previous[curr];
    }
    return true;
  } else {
    return false;
  }
}

bool IncreaseCapacity(VVi &capacity, VVi &flow, unsigned u, unsigned v) {
  ++capacity[u][v];
  return ImproveFlow(0, capacity.size() - 1, capacity, flow);
}

bool DecreaseCapacity(VVi &capacity, VVi &flow, unsigned u, unsigned v) {
  --capacity[u][v];
  if (flow[u][v] > capacity[u][v]) {
      --flow[u][v];
      ++flow[v][u];

    auto path = FindFlowPath(0, u, capacity, flow);
    int curr = u;
    int prev = path[curr];
    while (prev != -1) {
      --flow[prev][curr];
      ++flow[curr][prev];
      curr = prev;
      prev = path[curr];
    }
    path = FindFlowPath(v, capacity.size() - 1, capacity, flow);
    curr = capacity.size() - 1;
    prev = path[curr];
    while (prev != -1) {
      --flow[prev][curr];
      ++flow[curr][prev];
      curr = prev;
      prev = path[curr];
    }
    return !ImproveFlow(0, capacity.size() - 1, capacity, flow);
  } else {
    return false;
  }
}

int main() {
  unsigned n;
  std::cin >> n;

  unsigned m;
  std::cin >> m;
  unsigned s = 0;
  unsigned t = n - 1;

  auto capacity = scan(n, m);

  vector<vector<int>> flow(n, vector<int>(n, 0));

  while(ImproveFlow(s, t, capacity, flow))
    ;
  int flow_size = 0;
  for (auto f : flow[s])
    flow_size += f;

  std::cout << flow_size << std::endl;

  unsigned n_operations;
  std::cin >> n_operations;
  for (unsigned i = 0; i < n_operations; ++i) {
    unsigned operation, u, v;
    std::cin >> operation >> u >> v;
    --u;
    --v;
    switch (operation) {
      case 1:
        if (IncreaseCapacity(capacity, flow, u, v))
          ++flow_size;
        break;
      case 2:
        if (DecreaseCapacity(capacity, flow, u, v))
          --flow_size;
        break;
    }
    std::cout << flow_size << std::endl;
  }
  return 0;
}
