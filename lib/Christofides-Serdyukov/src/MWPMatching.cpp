#include "MWPMatching.hpp"

#include <cassert>
#include <vector>

#define PERFECT_MATCHING_DOUBLE
#include "PerfectMatching.h"

std::vector<int> BlossomMWPM(int oddSize, const float *subAdjMatrix) {
  if (oddSize <= 0) {
    return {};
  }
  if (oddSize % 2 != 0) {
    return {oddSize, -1};
  }

  const int maxEdges = (oddSize * (oddSize - 1)) / 2;

  PerfectMatching pm(oddSize, maxEdges);

  pm.options.verbose = false;

  for (int i = 0; i < oddSize; ++i) {
    for (int j = i + 1; j < oddSize; ++j) {
      double w = static_cast<double>(
          subAdjMatrix[(size_t)i * (size_t)oddSize + (size_t)j]);
      pm.AddEdge(i, j, w);
    }
  }

  pm.Solve();

  std::vector<int> sub_matching(oddSize, -1);

  for (int i = 0; i < oddSize; ++i) {
    int mate = pm.GetMatch(i);
    if (mate < 0) {
      sub_matching[(size_t)i] = -1;
    } else {
      sub_matching[(size_t)i] = mate;
    }
  }

  return sub_matching;
}
