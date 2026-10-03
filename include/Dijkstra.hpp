#pragma once

#include "ContinentGraph.hpp"

#include <limits>
#include <optional>
#include <vector>

namespace graph {

struct DijkstraResult {
    // std::numeric_limits<int>::max() denotes unreachable vertices
    static constexpr int infinity = std::numeric_limits<int>::max();

    std::vector<int> distances;
    std::vector<std::size_t> predecessors;
    std::size_t source;

    // Returns the path from source to target, or std::nullopt if unreachable
    [[nodiscard]] std::optional<std::vector<std::size_t>> 
    getPathTo(std::size_t target) const;
};

// Computes single-source shortest paths from `startVertex`
[[nodiscard]] DijkstraResult 
dijkstra(const ContinentGraph& graph, std::size_t startVertex);

} // namespace graph