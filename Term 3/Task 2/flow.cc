#include <iostream>
#include <vector>
#include <queue>
#include <climits>

using std::vector;

using VVi = vector<vector<int>>;

vector<vector<int>> scan(unsigned n, unsigned E) {
  vector<vector<int>> capacity(n, vector<int>(n, 0));

  for (unsigned i = 0; i < E; ++i) {
    unsigned u, v, c;
    std::cin >> u >> v >> c;
    capacity[u - 1][v - 1] += c;
    capacity[v - 1][u - 1] += c;
  }

  return capacity;
}

bool ImproveFlow(unsigned s, unsigned t, const VVi &capacity, VVi &flow) {
  unsigned n = capacity.size();
  vector<int> previous(n, -1);
  std::queue<unsigned> pending;
  pending.push(s);
  vector<bool> visited(n, false);
  visited[s] = true;

  while (!pending.empty()) {
    unsigned u = pending.front();
    pending.pop();
    for (unsigned v = 0; v < n; ++v) {
      if (capacity[u][v] - flow[u][v] && !visited[v]) {
        visited[v] = true;
        previous[v] = u;
        pending.push(v);
        if (v == t)
          break;
      }
    }
  }

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

int main() {
  unsigned n;
  std::cin >> n;

  while (n) {
    unsigned s, t, E;
    std::cin >> s >> t >> E;
    --s;
    --t;

    auto capacity = scan(n, E);

    vector<vector<int>> flow(n, vector<int>(n, 0));

    while(ImproveFlow(s, t, capacity, flow))
      ;
    int flow_size = 0;
    for (auto f : flow[s])
      flow_size += f;

    std::cout << flow_size << std::endl;
    std::cin >> n;
  }
  return 0;
}
