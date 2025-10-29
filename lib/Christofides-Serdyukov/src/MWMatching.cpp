#include "MWMatching.hpp"

#include <iostream>
#include <limits>
#include <map>

std::vector<Edge> GreedyPerfectMatching(const std::vector<int> &h_odd_vertices,
                                        const float *h_adjMatrix,
                                        size_t numVerts) {
  std::cout << "  [CPU] Running (sub-optimal) greedy perfect matching..."
            << std::endl;

  std::vector<Edge> h_matching;
  std::map<int, bool> matched;
  for (int v_id : h_odd_vertices) {
    matched[v_id] = false;
  }

  for (int i = 0; i < h_odd_vertices.size(); ++i) {
    int u = h_odd_vertices[i];
    if (matched[u])
      continue;

    float min_dist = std::numeric_limits<float>::max();
    int v_best = -1;

    for (int j = i + 1; j < h_odd_vertices.size(); ++j) {
      int v = h_odd_vertices[j];
      if (!matched[v]) {
        // Read distance from the adjacency matrix
        float d = h_adjMatrix[u * numVerts + v];
        if (d < min_dist) {
          min_dist = d;
          v_best = v;
        }
      }
    }

    if (v_best != -1) {
      h_matching.push_back({u, v_best, min_dist});
      matched[u] = true;
      matched[v_best] = true;
    }
  }
  return h_matching;
}
