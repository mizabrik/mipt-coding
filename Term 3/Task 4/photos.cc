#include <climits>
#include <cstdio>

#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>
#include <queue>

using std::vector;
using ll = long long;
using ull = unsigned long long;
using uvector = std::vector<ull>;

class Network {
 public:
  Network(ull size, ull source, ull sink) :
      flow_cost(0),
      size_(size),
      source_(source),
      sink_(sink),
      adjacent_arcs_(size) {}

  void AddArc(ll from, ll to, ll capacity, ll price, ull id) {
    adjacent_arcs_[from].push_back(arcs_.size());
    arcs_.emplace_back(from, to, capacity, price, 0, id);
    adjacent_arcs_[to].push_back(arcs_.size());
    arcs_.emplace_back(to, from, 0, -price, 0, id);
  }

  ll FlowValue() const {
    ll value = 0;

    for (auto arc : arcs_)
      if (arc.from == source_) {
        value += arc.flow;
      }

    return value;
  }

  void Maximize() {
    while (ImproveFlow())
      ;
  }

  ll FlowCost() const {
    ll cost = 0;

    for (auto arc : arcs_) {
      if (arc.flow > 0) {
        cost += arc.flow * arc.price;
      }
    }
    
    return cost;
  }

  void PrintChosen() {
    std::vector<int> plan((size_ - 2) /2, 0);
    for (auto arc : arcs_) {
      if (arc.flow > 0 && arc.id) {
        plan[arc.id - 1] = 1;
      }
    }
    for (auto i : plan) {
      std::cout << i << ' ';
    }
    std::cout << std::endl;
  }

 private:
  bool ImproveFlow() {
    std::vector<ll> distance(size_, LLONG_MAX);
    std::vector<ll> leading_arc(size_, -1);
    distance[source_] = 0;

    bool changed = true;
    for (ull step = 0; step < size_ && changed; ++step) {
      changed = false;
      for (ull id = 0; id < arcs_.size(); ++id) {
        auto arc = arcs_[id];
        if (distance[arc.from] < LLONG_MAX && arc.ResidualCapacity()) {
          ll new_distance = distance[arc.from] + arc.price;
          if (new_distance < distance[arc.to]) {
            changed = true;
            distance[arc.to] = new_distance;
            leading_arc[arc.to] = id;
          }
        }
      }
    }

    ull node = sink_;
    while (leading_arc[node] != -1) {
      ModifyFlow(leading_arc[node], 1);
      flow_cost += arcs_[leading_arc[node]].price;
      node = arcs_[leading_arc[node]].from;
    }

    return leading_arc[sink_] != -1;
  }

  ll flow_cost;

  // Assumes there are no cycles of negative value
  std::vector<ull> MinimalPath(ull from, ull to) {
    std::vector<ll> distance(size_, LLONG_MAX);
    std::vector<ll> leading_arc(size_, -1);
    distance[from] = 0;

    for (ull step = 0; step < size_; ++step) {
      for (ull id = 0; id < arcs_.size(); ++id) {
        auto arc = arcs_[id];
        if (distance[arc.from] < LLONG_MAX && arc.ResidualCapacity()) {
          ll new_distance = distance[arc.from] + arc.price;
          if (new_distance < distance[arc.to]) {
            distance[arc.to] = new_distance;
            leading_arc[arc.to] = id;
          }
        }
      }
    }

    vector<ull> answer;
    ull node = to;
    while (leading_arc[node] != -1) {
      answer.push_back(leading_arc[node]);
      node = arcs_[leading_arc[node]].from;
    }
    std::reverse(answer.begin(), answer.end());

    return answer;
  }

  struct Arc {
    Arc(ull from, ull to, ll capacity, ll price, ll flow, ull id)
        : from(from), to(to), capacity(capacity), price(price), flow(flow), id(id) {}

    ll ResidualCapacity() const {
      return capacity - flow;
    }

    ull from;
    ull to;
    ll capacity;
    ll price;
    ll flow;
    ull id;
  };

  void ModifyFlow(ull arc_id, ll delta) {
    arcs_[arc_id].flow += delta;
    arcs_[ReverseArcId(arc_id)].flow -= delta;
  }

  static ll ReverseArcId(ull arc_id) {
    return arc_id ^ 1;
  }

  ull size_;
  ull source_;
  ull sink_;
  std::vector<Arc> arcs_;
  std::vector<std::vector<ull>> adjacent_arcs_;
};

struct Task {
  Task (ll id, ll start, ll finish, ll price)
      : id(id), start(start), finish(finish), price(price) {}
  ll id;
  ll start;
  ll finish;
  ll price;

  bool operator <(const Task &other) const {
    return start < other.start || (start == other.start && finish < other.finish)
        || (start == other.start && finish == other.finish && id < other.id);
  }
};

int main() {
  ll n, k, m;
  std::cin >> n >> k >> m;

  vector<ll> costs;
  for (ll i = 0; i < m; ++i) {
    ll cost;
    std::cin >> cost;
    costs.push_back(cost);
  }

  ll girl_entries = 0;
  std::vector<vector<ll>> photos(n);
  for (ll i = 0; i < n; ++i) {
    ll g_count, girl;
    std::cin >> g_count;
    girl_entries += g_count;
    for (ll j = 0; j < g_count; ++j) {
      std::cin >> girl;
      photos[i].push_back(girl - 1);
    }
  }

  Network net(girl_entries + n + 1, 0, girl_entries + n);

  for (ll photo = 0; photo < n; ++photo) {
  }

  return 0;
}
