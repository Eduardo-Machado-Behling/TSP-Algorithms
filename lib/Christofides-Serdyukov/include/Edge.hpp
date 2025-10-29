#ifndef EDGE_HPP
#define EDGE_HPP

struct Edge {
  int src;
  int dst;
  float weight;

  bool operator<(const Edge &other) const { return weight < other.weight; }

  bool operator>(const Edge &other) const { return weight > other.weight; }
};

#endif // EDGE_HPP
