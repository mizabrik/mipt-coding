#include <climits>
#include <algorithm>
#include <iostream>
#include <vector>
#include <queue>

using std::vector;
using uvector = std::vector<unsigned>;

unsigned CountDistances(const std::string &str, const std::string &pattern,
                        unsigned str_offset, std::vector<uvector> &neighbours,
                        uvector &distance_0, uvector &distance_1) {
  unsigned distance_const = 0;
  unsigned pattern_wildcard = 0;

  for (unsigned i = 0; i < pattern.size(); ++i) {
    unsigned bound = str.size() + i - pattern.size();
    for (unsigned j = i; j <= bound; ++j) {
      unsigned str_wildcard = std::count(str.begin(), str.begin() + j, '?');
      switch (pattern[i]) {
        case '0':
          if (str[j] == '?') {
            ++distance_1[str_offset + str_wildcard];
            ++str_wildcard;
          } else if (pattern[i] != str[j]) {
            ++distance_const;
          }
          break;
        case '1':
          if (str[j] == '?') {
            ++distance_0[str_offset + str_wildcard];
            ++str_wildcard;
          } else if (pattern[i] != str[j]) {
            ++distance_const;
          }
          break;
        case '?':
          switch (str[j]) {
            case '0':
              ++distance_1[pattern_wildcard];
              break;
            case '1':
              ++distance_0[pattern_wildcard];
              break;
            case '?':
              neighbours[pattern_wildcard].push_back(str_offset + str_wildcard);
              ++str_wildcard;
              break;
          }
          break;
      }
    }
    if (pattern[i] == '?')
      ++pattern_wildcard;
  }

  return distance_const;
}

vector<unsigned> CodeWildcards(const std::string &str) {
  vector<unsigned> numbering;

  for (unsigned i = 0; i < str.size(); ++i)
    if (str[i] == '?')
      numbering.push_back(i);

  return numbering;
}

// Flow network
class Network {
 public:
  Network(unsigned size, unsigned source, unsigned sink) :
      size_(size),
      source_(source),
      sink_(sink),
      arcs_(size),
      flow_(size, uvector(size, 0)) {}

  void AddArc(unsigned from, unsigned to, unsigned capacity) {
    arcs_[from].emplace_back(from, to, capacity);
  }

  int FlowValue() const {
    int value = 0;

    for (auto subflow : flow_[source_])
      value += subflow;

    return value;
  }

  void Maximize() {
    while (ImproveByBlockingFlow())
      ;
  }

  // Finds minimum cat (S, T) and returns vector of size |V|
  // where vector[v_i] == true <=> v_i in S
  vector<bool> GetMinCut() {
    vector<bool> cut(size_, false);

    Maximize();
    ResidualDFS(source_, cut);

    return cut;
  }

 private:
  struct Arc {
    Arc(unsigned from, unsigned to, unsigned capacity)
        : from(from), to(to), capacity(capacity) {}
    unsigned from;
    unsigned to;
    unsigned capacity;
  };

  void MaximizeByPath() {
    while(ImproveByPath());
  }

  unsigned ResidualCapacity(const Arc &arc) const {
    return arc.capacity - flow_[arc.from][arc.to];
  }

  Network GetLevelNetwork() const {
    Network net(size_, source_, sink_);
    
    std::queue<unsigned> pending;
    pending.push(source_);
    std::vector<int> distance(size_, -1);
    distance[source_] = 0;


    while (!pending.empty()) {
      unsigned u = pending.front();
      pending.pop();
      if (u == sink_)
        break;
      for (auto arc : arcs_[u]) {
        if (ResidualCapacity(arc)) {
          if (distance[arc.to] < 0) {
            distance[arc.to] = distance[u] + 1;
            pending.push(arc.to);
          }
          if (distance[arc.to] > distance[u])
            net.AddArc(arc.from, arc.to, ResidualCapacity(arc));
        }
      }
    }

    return net;
  }

  void ApplyBlockingFlow(const Network &net) {
    for (unsigned u = 0; u < net.size_; ++u) {
      for (auto arc : net.arcs_[u]) {
        flow_[u][arc.to] += net.flow_[u][arc.to];
        flow_[arc.to][u] += net.flow_[arc.to][u];
      }
    }
  }

  bool ImproveByBlockingFlow() {
    Network net = GetLevelNetwork();
    net.MaximizeByPath();

    if (net.FlowValue() > 0) {
      ApplyBlockingFlow(net);
      return true;
    } else {
      return false;
    }
  }

  void ResidualDFS(unsigned node, vector<bool> &used) const {
    if (used[node])
      return;
    used[node] = true;

    for (auto arc : arcs_[node])
      if (ResidualCapacity(arc))
        ResidualDFS(arc.to, used);
  }

  std::vector<Arc> FindResidualPath(unsigned from, unsigned to) const {
    vector<const Arc *> previous(size_, nullptr);
    std::queue<unsigned> pending;
    pending.push(from);
    vector<bool> visited(size_, false);
    visited[from] = true;

    while (!pending.empty()) {
      unsigned u = pending.front();
      pending.pop();
      for (auto &arc : arcs_[u]) {
        unsigned v = arc.to;
        if (ResidualCapacity(arc) && !visited[v]) {
          visited[v] = true;
          previous[v] = &arc;
          pending.push(v);
          if (v == to)
            break;
        }
      }
    }

    vector<Arc> path;
    unsigned v = to;
    while (previous[v]) {
      path.push_back(*previous[v]);
      path.back().capacity = ResidualCapacity(path.back());
      v = previous[v]->from;
    }
    std::reverse(path.begin(), path.end());

    return path;
  }

  unsigned MinCapacity(const vector<Arc> &arcs) const {
    unsigned min_capacity = UINT_MAX;
    for (auto arc : arcs)
      min_capacity = std::min(min_capacity, arc.capacity);
    return min_capacity;
  }

  bool ImproveByPath() {
    auto path = FindResidualPath(source_, sink_);

    auto min_capacity = MinCapacity(path);

    if (!path.empty()) {
      for (auto arc : path) {
        flow_[arc.from][arc.to] += min_capacity;
        flow_[arc.to][arc.from] -= min_capacity;
      }

      return true;
    } else {
      return false;
    }
  }

  unsigned size_;
  unsigned source_;
  unsigned sink_;
  std::vector<std::vector<Arc>> arcs_;
  std::vector<uvector> flow_;
};

// Replace wildcards in a way, minimizing distance between str and pattern.
// Returns distance between updated strings.
unsigned MakeClosest(std::string &str, std::string &pattern) {
  auto pattern_wildcards = CodeWildcards(pattern);
  auto str_wildcards = CodeWildcards(str);
  unsigned n_wildcards = pattern_wildcards.size() + str_wildcards.size();

  vector<unsigned> distance_0(n_wildcards, 0);
  vector<unsigned> distance_1(n_wildcards, 0);
  vector<vector<unsigned>> neighbours(pattern_wildcards.size());

  unsigned str_offset = pattern_wildcards.size();
  unsigned distance_const = CountDistances(str, pattern, str_offset,
                                           neighbours, distance_0, distance_1);

  unsigned source = n_wildcards;
  unsigned sink = n_wildcards + 1;
  Network net(n_wildcards + 2, source, sink);
  for (unsigned wildcard = 0; wildcard < n_wildcards; ++wildcard) {
    if (wildcard < pattern_wildcards.size()) {
      for (auto neighbour : neighbours[wildcard]) {
        net.AddArc(wildcard, neighbour, 1);
        net.AddArc(neighbour, wildcard, 1);
      }
    }
    net.AddArc(source, wildcard, distance_0[wildcard]);
    net.AddArc(wildcard, sink, distance_1[wildcard]);
  }

  auto wildcard_solution = net.GetMinCut();
  for (unsigned wildcard = 0; wildcard < pattern_wildcards.size(); ++wildcard)
    pattern[pattern_wildcards[wildcard]] = wildcard_solution[wildcard] ?
        '1' : '0';
  for (unsigned wildcard = 0; wildcard < str_wildcards.size(); ++wildcard)
    str[str_wildcards[wildcard]] = wildcard_solution[str_offset + wildcard] ?
        '1' : '0';

  return distance_const + net.FlowValue();
}

int main() {
  std::string str, pattern;
  std::cin >> str >> pattern;

  std::cout << MakeClosest(str, pattern) << std::endl;
  std::cout << str << std::endl << pattern << std::endl;
  return 0;
}
