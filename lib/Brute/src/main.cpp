#include "lib.h"

#include "limits.h"

#include <algorithm>
#include <iostream>
#include <vector>

size_t TSP(int *output, const float *adjMatrix, size_t numVerts) {
  std::vector<int> permutation;
  std::vector<int> bestRoute;
  size_t bestRouteCost = INT_MAX;
  int routeCost = 0;

  for (size_t i = 0; i < numVerts; i++) {
    permutation.push_back(i);
  }

  // O(n!)
  do {
    routeCost = 0;
    // O(n)
    for (size_t index = 1; index < numVerts; index++) {
      routeCost +=
          adjMatrix[permutation[index - 1] * numVerts + permutation[index]];
    }
    routeCost +=
        adjMatrix[permutation[numVerts - 1] * numVerts + permutation[0]];
    if (routeCost < bestRouteCost) {
      bestRoute = permutation;
      bestRouteCost = routeCost;
    }
  } while (std::next_permutation(permutation.begin(), permutation.end()));

  for (size_t index = 0; index < numVerts; index++) {
    output[index] = bestRoute[index];
  }

  return numVerts;
}
