#include "MWPMatching.hpp"

#include <cassert>
#include <vector>

#define PERFECT_MATCHING_DOUBLE
#include "PerfectMatching.h" // path should resolve to blossom5's header

int BlossomMWPM(int oddSize, const float *subAdjMatrix,
                std::vector<int> &sub_matching) {
  if (oddSize <= 0) {
    sub_matching.clear();
    return 0;
  }
  if (oddSize % 2 != 0) {
    sub_matching.assign((size_t)oddSize, -1);
    return 0;
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

  sub_matching.assign((size_t)oddSize, -1);

  int pairs = 0;
  for (int i = 0; i < oddSize; ++i) {
    int mate = pm.GetMatch(i);
    if (mate < 0) {
      sub_matching[(size_t)i] = -1;
    } else {
      sub_matching[(size_t)i] = mate;
      if (i < mate)
        ++pairs; // count each pair once
    }
  }

  return pairs;
}
