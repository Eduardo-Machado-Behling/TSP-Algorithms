#include "lib.h"

#include <algorithm>
#include <iostream>
#include <stack>
#include <vector>

struct Edge {
  int src;
  int dst;
  float weight;

  Edge(size_t src, size_t dst, float weight)
      : src(src), dst(dst), weight(weight) {}

  bool operator<(const Edge &other) const { return weight < other.weight; }

  bool operator>(const Edge &other) const { return weight > other.weight; }
};

class DisjointSet {
private:
  std::vector<int> parent;
  std::vector<int> rank;

public:
  explicit DisjointSet(int n) : parent(n), rank(n, 0) {
    for (int i = 0; i < n; ++i)
      parent[i] = i;
  }

  int find(int x) {
    if (parent[x] != x)
      parent[x] = find(parent[x]); // path compression
    return parent[x];
  }

  void unite(int x, int y) {
    int rx = find(x);
    int ry = find(y);
    if (rx == ry)
      return;

    if (rank[rx] < rank[ry])
      parent[rx] = ry;
    else if (rank[rx] > rank[ry])
      parent[ry] = rx;
    else {
      parent[ry] = rx;
      ++rank[rx];
    }
  }
};

std::vector<Edge> KruskalMST(const std::vector<Edge> &edges, int numVerts) {
  std::vector<Edge> result;
  result.reserve(numVerts - 1);

  std::vector<Edge> sorted = edges;
  std::sort(sorted.begin(), sorted.end(),
            [](const Edge &a, const Edge &b) { return a.weight < b.weight; });

  DisjointSet dsu(numVerts);

  for (const auto &e : sorted) {
    int setU = dsu.find(e.src);
    int setV = dsu.find(e.dst);

    if (setU != setV) {
      result.push_back(e);
      dsu.unite(setU, setV);

      if ((int)result.size() == numVerts - 1)
        break;
    }
  }

  return result;
}

static std::vector<std::vector<std::pair<int, float>>>
buildWeightedAdjListFromMST(const std::vector<Edge> &mstEdges, int numVerts) {
  std::vector<std::vector<std::pair<int, float>>> adj(numVerts);
  for (const auto &e : mstEdges) {
    adj[e.src].emplace_back(e.dst, e.weight);
    adj[e.dst].emplace_back(e.src, e.weight);
  }
  return adj;
}

static void
dfsWeighted(int start,
            const std::vector<std::vector<std::pair<int, float>>> &adj,
            std::vector<int> &path) {
  std::vector<bool> visited(adj.size(), false);
  std::stack<int> st;
  st.push(start);

  while (!st.empty()) {
    int v = st.top();
    st.pop();

    if (!visited[v]) {
      visited[v] = true;
      path.push_back(v);

      std::vector<std::pair<int, float>> neighbors = adj[v];
      std::sort(neighbors.begin(), neighbors.end(),
                [](auto &a, auto &b) { return a.second < b.second; });

      for (auto it = neighbors.rbegin(); it != neighbors.rend(); ++it)
        if (!visited[it->first])
          st.push(it->first);
    }
  }
}

size_t TSP(int *output, const float *adjMatrix, size_t numVerts) {
  if (numVerts == 0)
    return 0.0f;

  std::vector<Edge> edges;
  edges.reserve(numVerts * (numVerts - 1) / 2);

  for (size_t i = 0; i < numVerts; ++i) {
    for (size_t j = i + 1; j < numVerts; ++j) {
      float w = adjMatrix[i * numVerts + j];
      edges.emplace_back(i, j, w);
    }
  }
  std::vector<Edge> mstEdges = KruskalMST(edges, static_cast<int>(numVerts));

  auto adj = buildWeightedAdjListFromMST(mstEdges, static_cast<int>(numVerts));

  std::vector<int> path;
  path.reserve(numVerts + 1);
  dfsWeighted(0, adj, path);

  for (size_t i = 0; i < path.size(); ++i)
    output[i] = path[i];

  return numVerts;
}
