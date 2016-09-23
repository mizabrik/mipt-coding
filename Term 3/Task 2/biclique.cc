#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using std::vector;

vector<vector<unsigned long>> adjacent;

// Returns size of larger component
void scan(unsigned M, unsigned N) {
  std::string guy_mates;
  vector<bool> acquaintant;
  adjacent.clear();

  for (unsigned guy = 0; guy < M; ++guy) {
    acquaintant.assign(N, false);
    adjacent.emplace_back();
    unsigned girl;
    std::cin >> girl;
    while (girl) {
      acquaintant[girl - 1] = true;
      std::cin >> girl;
    }
    for (girl = 0; girl < acquaintant.size(); ++girl) 
      if (!acquaintant[girl])
        adjacent.back().push_back(girl);
  }
}

vector<bool> used;
vector<int> match;

bool FindChain(unsigned v) {
  if (used[v])
    return false;
  used[v] = true;

  for (auto neighbour : adjacent[v]) {
    if (match[neighbour] < 0 || FindChain(match[neighbour])) {
      match[neighbour] = v;
      return true;
    }
  }

  return false;
}

void GetMatch(unsigned A, unsigned B) {
  vector<bool> preused(A, false);
  match.assign(B, -1);

  for (unsigned v = 0; v < A; ++v) {
    for (auto neighbour : adjacent[v]) {
      if (match[neighbour] < 0) {
        match[neighbour] = v;
        preused[v] = true;
        break;
      }
    }
  }
  for (unsigned v = 0; v < A; ++v) {
    if (!preused[v]) {
      used.assign(A, false);
      FindChain(v);
    }
  }
}

vector<int> markA;
vector<int> markB;

void DFS(unsigned v) {
  if (markA[v])
    return;
  markA[v] = true;

  for (auto u : adjacent[v]) {
    if (match[u] != (int) v && !markB[u]) {
      markB[u] = true;
      if (match[u] >= 0)
        DFS(match[u]);
    }
  }
}

int main() {
  unsigned k;
  std::cin >> k;

  for (unsigned i = 0; i < k; ++i) {
    unsigned A, B;
    std::cin >> A >> B;
    scan(A, B);

    GetMatch(A, B);

    vector<bool> A_free(A, true);
    for (auto v : match)
      if (v >= 0)
        A_free[v] = false;

    markA.assign(A, false);
    markB.assign(B, false);

    for (unsigned v = 0; v < A; ++v)
      if (A_free[v])
        DFS(v);

    unsigned markedA = std::count(markA.begin(), markA.end(), true);
    unsigned markedB = std::count(markB.begin(), markB.end(), false);
    std::cout << (markedA + markedB) << std::endl << markedA << ' ' << markedB << std::endl;
    for (unsigned v = 0; v < A; ++v)
      if (markA[v])
        std::cout << (v + 1) << ' ';
    std::cout << std::endl;
    for (unsigned u = 0; u < B; ++u)
      if (!markB[u])
        std::cout << (u + 1) << ' ';
    std::cout << std::endl;
    std::cout << std::endl;
  }

  return 0;
}
