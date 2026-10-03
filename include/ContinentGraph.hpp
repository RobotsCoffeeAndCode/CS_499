#pragma once

#include <cstddef>
#include <vector>

namespace graph {

struct Edge {
  std::size_t to{0};
  int weight{0};
};

class ContinentGraph {

public:
  using VertexId = std::size_t;

  // Explicit constructor to prevent implicit conversions from integers
  explicit ContinentGraph(std::size_t vertices);

  void addEdge(VertexId start, VertexId end, int weight);

  [[nodiscard]] const std::vector<Edge> &getNeighbors(VertexId currentVertex) const;

  [[nodiscard]] std::size_t getVertexCount() const noexcept;

  void printGraph() const;

private:
  std::size_t numVertices_{0};
  std::vector<std::vector<Edge>> adjList_;
};

} // namespace graph