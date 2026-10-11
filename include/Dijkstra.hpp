// Dijkstra.hpp
#pragma once

#include "ContinentGraph.hpp"

#include <limits>
#include <optional>
#include <vector>

namespace graph {

// Holds shortest paths, predecessor tracks, and the source vertex
struct DijkstraResult {
  static constexpr int infinity = std::numeric_limits<int>::max();
  static constexpr std::size_t kInvalidVertex = std::numeric_limits<std::size_t>::max();

  std::vector<int> distances;
  std::vector<std::size_t> predecessors;
  std::size_t source{0};

  // Reconstructs the shortest path sequence from source to target
  [[nodiscard]] std::optional<std::vector<std::size_t>>
  getPathTo(std::size_t target) const;
};

// Computes single-source shortest paths on the continent graph
[[nodiscard]] DijkstraResult
dijkstra(const ContinentGraph &graph, std::size_t startVertex);

} // namespace graph