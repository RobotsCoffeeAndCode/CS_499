// ContinentGraph.hpp
#pragma once

#include <cstddef>
#include <vector>

namespace graph {

// Weighted directed edge in the continent graph
struct Edge {
  std::size_t to{0};
  int weight{0};
};

// Adjacency-list graph representing continents and transit costs
class ContinentGraph {
public:
  using VertexId = std::size_t;

  explicit ContinentGraph(std::size_t vertices);

  // Adds an undirected, weighted connection between two vertices
  void addEdge(VertexId start, VertexId end, int weight);

  // Gets the outgoing edges for a given vertex
  [[nodiscard]] const std::vector<Edge> &getNeighbors(VertexId currentVertex) const;

  [[nodiscard]] std::size_t getVertexCount() const noexcept;

  // Prints all vertices and their connected edges to stdout
  void printGraph() const;

private:
  std::size_t numVertices_{0};
  std::vector<std::vector<Edge>> adjList_;
};

} // namespace graph