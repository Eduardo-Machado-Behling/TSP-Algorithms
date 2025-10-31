#include "lib.h"

#include <iostream>
#include <vector>
#include <algorithm>

std::vector<size_t> twoOpt(std::vector<size_t> route, const float *adjMatrix,
                           size_t numVerts);

size_t calculateBetter(const std::vector<size_t> &route, const float *adjMatrix,
                       size_t numVerts);

size_t TSP(int *output, const float *adjMatrix, size_t numVerts) {
  std::vector<size_t> route(numVerts + 1);
  for (size_t i = 0; i < numVerts; ++i)
    route[i] = i;
  route[numVerts] = 0;

  route = twoOpt(route, adjMatrix, numVerts);
  for (size_t i = 0; i < numVerts; i++)
    output[i] = route[i];
  size_t cost = calculateBetter(route, adjMatrix, numVerts);
  return numVerts;
}

inline float getDist(const float *adjMatrix, size_t numVerts, size_t i,
                     size_t j) {
  return adjMatrix[i * numVerts + j];
}

size_t calculateBetter(const std::vector<size_t> &route, const float *adjMatrix,
                       size_t numVerts) {
  size_t cost = 0;
  for (size_t i = 0; i < route.size() - 1; ++i)
    cost += getDist(adjMatrix, numVerts, route[i], route[i + 1]);

  return cost;
}

std::vector<size_t> twoOpt(std::vector<size_t> route, const float *adjMatrix,
                           size_t numVerts) {
  bool b = true;
  size_t better = calculateBetter(route, adjMatrix, numVerts);

  while (b) {
    b = false;

    for (size_t i = 1; i < numVerts - 2; ++i) {
      for (size_t k = i + 1; k < numVerts - 1; ++k) {

        size_t A = route[i - 1];
        size_t B = route[i];
        size_t C = route[k];
        size_t D = route[k + 1];

        float delta = (getDist(adjMatrix, numVerts, A, C) +
                       getDist(adjMatrix, numVerts, B, D)) -
                      (getDist(adjMatrix, numVerts, A, B) +
                       getDist(adjMatrix, numVerts, C, D));

        if (delta < 0) {
          std::reverse(route.begin() + i, route.begin() + k + 1);
          better += delta;
          b = true;
        }
      }
    }
  }

  return route;
}