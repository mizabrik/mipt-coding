#include <algorithm>
#include <iostream>
#include <vector>

using std::vector;

vector<vector<unsigned long>> adjacency;

// Returns size of larger component
unsigned long scan(unsigned long N, unsigned long M) {
  vector<vector<unsigned long>> floor(N, vector<unsigned long>(M, 0));

  int odd = 0, even = 0;
  for (unsigned long i = 0; i < N; ++i) {
    for (unsigned long j = 0; j < M; ++j) {
      char c;
      std::cin >> c;
      if (c == '*')
        floor[i][j] = (i + j) % 2 ? ++odd : ++even;
    }
  }
  
  int offset = even < odd ? 0 : 1;
  for (unsigned long i = 0; i < N; ++i) {
    for (unsigned long j = i % 2 ? 1 - offset : offset; j < M; j += 2) {
      if (floor[i][j]) {
        adjacency.emplace_back();
        if (i > 0 && floor[i - 1][j])
          adjacency.back().push_back(floor[i - 1][j] - 1);
        if (j + 1 < M && floor[i][j + 1])
          adjacency.back().push_back(floor[i][j + 1] - 1);
        if (i + 1 < N && floor[i + 1][j])
          adjacency.back().push_back(floor[i + 1][j] - 1);
        if (j > 0 && floor[i][j - 1])
          adjacency.back().push_back(floor[i][j - 1] - 1);
      }
    }
  }

  return std::max(even, odd);
}

vector<bool> used;
vector<int> match;

bool FindChain(unsigned long v) {
  if (used[v])
    return false;
  used[v] = true;

  for (auto neighbour : adjacency[v]) {
    if (match[neighbour] < 0 || FindChain(match[neighbour])) {
      match[neighbour] = v;
      return true;
    }
  }

  return false;
}

int main() {
  unsigned long N, M;
  long priceA, priceB;

  std::cin >> N >> M >> priceA >> priceB;

  unsigned long B = scan(N, M);
  unsigned long A = adjacency.size();
  unsigned long broken = A + B;

  if (2 * priceB <= priceA) {
    std::cout << (priceB * broken) << std::endl;
  } else {
    vector<bool> preused(A, false);
    match.assign(B, -1);

    for (unsigned long v = 0; v < A; ++v) {
      for (auto neighbour : adjacency[v]) {
        if (match[neighbour] < 0) {
          match[neighbour] = v;
          preused[v] = true;
          break;
        }
      }
    }
    for (unsigned long v = 0; v < A; ++v) {
      if (!preused[v]) {
        used.assign(A, false);
        FindChain(v);
      }
    }
    unsigned long pairs = B - std::count(match.begin(), match.end(), -1);
    unsigned long single = broken - 2 * pairs;
    std::cout << (pairs * priceA + single * priceB) << std::endl;
  }

  return 0;
}
