#ifndef MW_MATCHING_HPP
#define MW_MATCHING_HPP

#include <vector>

#include "Edge.hpp"

std::vector<Edge> GreedyPerfectMatching(const std::vector<int> &h_odd_vertices,
                                        const float *h_adjMatrix,
                                        size_t numVerts);

#endif // MW_MATCHING_HPP
