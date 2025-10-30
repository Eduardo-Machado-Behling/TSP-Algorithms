#include "MST.hpp"

#include <algorithm>
#include <iostream>

class DisjointSetUnion {
  std::vector<int> parent;
  std::vector<int> rank;

public:
  DisjointSetUnion(int n) {
    parent.resize(n);
    rank.resize(n);
    for (int i = 0; i < n; ++i) {
      parent[i] = i;
      rank[i] = 0;
    }
  }

  int find(int i) {
    if (parent[i] == i)
      return i;
    return parent[i] = find(parent[i]);
  }

  void unite(int i, int j) {
    int root_i = find(i);
    int root_j = find(j);
    if (root_i != root_j) {
      if (rank[root_i] < rank[root_j])
        parent[root_i] = root_j;
      else if (rank[root_i] > rank[root_j])
        parent[root_j] = root_i;
      else {
        parent[root_j] = root_i;
        rank[root_i]++;
      }
    }
  }
};

std::vector<Edge> KruskalMST(std::vector<Edge> &h_edge_list, int numVerts) {

  DisjointSetUnion dsu(numVerts);
  std::vector<Edge> h_mst_edges;
  h_mst_edges.reserve(numVerts - 1);

  for (const auto &edge : h_edge_list) {
    if (dsu.find(edge.src) != dsu.find(edge.dst)) {
      h_mst_edges.push_back(edge);
      dsu.unite(edge.src, edge.dst);
      if (h_mst_edges.size() == numVerts - 1) {
        break;
      }
    }
  }
  return h_mst_edges;
}
