#include "lib.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <stack>
#include <vector>

#include "Edge.hpp"
#include "MST.hpp"
#include "MWPMatching.hpp"

size_t TSP(int *output, const float *adjMatrix, size_t numVerts) {
  std::vector<Edge> edges;
  edges.reserve(numVerts * numVerts);
  for (size_t i = 0; i < numVerts; ++i) {
    for (size_t j = i + 1; j < numVerts; ++j) {
      edges.emplace_back(i, j, adjMatrix[i * numVerts + j]);
    }
  }
  std::sort(edges.begin(), edges.end()); // Sort by weight

  auto mst = KruskalMST(edges, numVerts);

  std::vector<int> vertDegrees(numVerts, 0); // MUST be initialized
  for (auto &edge : mst) {                   // Renamed from 'edges' to 'edge'
    vertDegrees[edge.src]++;
    vertDegrees[edge.dst]++;
  }

  std::vector<int> oddVerts;
  oddVerts.reserve(numVerts);
  for (size_t i = 0; i < numVerts; ++i) {
    if ((vertDegrees[i] & 1) != 0)
      oddVerts.push_back((int)i);
  }

  size_t oddSize = oddVerts.size();
  // If there are odd-degree vertices, build a reduced adjacency and run
  // matching.
  std::vector<int> sub_matching; // will be filled by Blossom function if used

  if (oddSize >= 2) {
    std::vector<float> subAdjMatrix(oddSize * oddSize, 0.0f);
    for (size_t i = 0; i < oddSize; ++i) {
      for (size_t j = i + 1; j < oddSize; ++j) {
        int main_i = oddVerts[i];
        int main_j = oddVerts[j];
        float w = adjMatrix[main_i * numVerts + main_j];
        subAdjMatrix[i * oddSize + j] = w;
        subAdjMatrix[j * oddSize + i] = w;
      }
    }

    sub_matching.assign((size_t)oddSize, -1);
    int match_count =
        BlossomMWPM((int)oddSize, subAdjMatrix.data(), sub_matching);
    (void)match_count; // not used further, but Blossom has filled sub_matching
  } else {
    // No matching needed
    sub_matching.assign(oddSize, -1);
  }

  std::vector<std::vector<int>> multigraph(numVerts);

  for (const auto &edge : mst) {
    multigraph[edge.src].push_back(edge.dst);
    multigraph[edge.dst].push_back(edge.src);
  }

  for (int i = 0; i < oddSize; ++i) {
    if (sub_matching[i] != -1 && i < sub_matching[i]) { // Add each edge once
      int main_src = oddVerts[i];
      int main_dst = oddVerts[sub_matching[i]];

      multigraph[main_src].push_back(main_dst);
      multigraph[main_dst].push_back(main_src);
    }
  }

  // Find a start vertex that has non-zero degree in the multigraph
  int start = -1;
  for (size_t v = 0; v < numVerts; ++v) {
    if (!multigraph[v].empty()) {
      start = (int)v;
      break;
    }
  }
  if (start == -1) {
    // No edges at all -> degenerate tour: output single vertex 0
    if (numVerts > 0) {
      output[0] = 0;
      return 1;
    }
    return 0;
  }

  std::vector<int> eulerian_circuit;
  std::stack<int> s;
  s.push(start);

  auto multigraph_copy = multigraph;

  while (!s.empty()) {
    int v = s.top();
    if (multigraph_copy[v].empty()) {
      eulerian_circuit.push_back(v);
      s.pop();
    } else {
      int u = multigraph_copy[v].back();
      multigraph_copy[v].pop_back();
      // remove the opposite edge occurrence
      auto &adj = multigraph_copy[u];
      for (size_t k = 0; k < adj.size(); ++k) {
        if (adj[k] == v) {
          adj[k] = adj.back(); // Swap with last
          adj.pop_back();      // Pop last
          break;
        }
      }
      s.push(u);
    }
  }

  // eulerian_circuit currently is in reverse order (end->start), reverse it
  std::reverse(eulerian_circuit.begin(), eulerian_circuit.end());

  // Convert Eulerian circuit to Hamiltonian tour by skipping repeated vertices
  std::vector<bool> visited((size_t)numVerts, false);
  size_t tour_idx = 0;
  int last_node = -1;

  for (int node : eulerian_circuit) {
    if (!visited[(size_t)node]) {
      if (last_node != -1) {
        // Optionally accumulate length: float tour_length +=
        // adjMatrix[last_node*numVerts + node];
      }
      visited[(size_t)node] = true;
      output[tour_idx++] = node;
      last_node = node;
    }
  }

  return numVerts;
}