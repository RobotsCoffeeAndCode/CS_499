// Dijkstra.cpp
#include "Dijkstra.hpp"
#include <algorithm>
#include <queue>
#include <stdexcept>

namespace graph {

// Internal node used for ordering the min-heap priority queue
struct QueueNode {
  int distance;
  std::size_t vertex;

  // Overload the > operator for node comparison
  bool operator>(const QueueNode &other) const noexcept {
    return distance > other.distance;
  }

};

// Implementation of Dijkstra's Algorithm, takes a graph and source vertex
// returns a DijkstraResult with costs to navigate to other nodes from source
DijkstraResult dijkstra(const ContinentGraph &graph, std::size_t startVertex) {
  const std::size_t vertexCount = graph.getVertexCount();
  if (startVertex >= vertexCount) {
    throw std::out_of_range("Start vertex index out of bounds.");
  }

  std::vector<int> distances(vertexCount, DijkstraResult::infinity);
  std::vector<std::size_t> predecessors(vertexCount, DijkstraResult::kInvalidVertex);

  // Min-heap ordered by smallest distance first
  std::priority_queue<QueueNode, std::vector<QueueNode>, std::greater<QueueNode>> prioq;

  distances[startVertex] = 0;
  prioq.push(QueueNode{.distance = 0, .vertex = startVertex});

  while (!prioq.empty()) {
    const auto [currentDist, u] = prioq.top();
    prioq.pop();

    // Skip processing if a shorter path to u has already been finalized
    if (currentDist > distances[u]) {
      continue;
    }

    for (const Edge &edge : graph.getNeighbors(u)) {
      if (edge.weight < 0) {
        throw std::invalid_argument("Dijkstra does not support negative weights.");
      }

      // Safe relaxation check preventing signed integer overflow
      if (distances[u] != DijkstraResult::infinity &&
          distances[u] + edge.weight < distances[edge.to]) {
        distances[edge.to] = distances[u] + edge.weight;
        predecessors[edge.to] = u;
        prioq.push(QueueNode{.distance = distances[edge.to], .vertex = edge.to});
      }
    }
  }

  return DijkstraResult{.distances = std::move(distances),
                        .predecessors = std::move(predecessors),
                        .source = startVertex};
}

// Walks backward from the target to the source using the predecessor tree
std::optional<std::vector<std::size_t>>
DijkstraResult::getPathTo(std::size_t target) const {
  if (target >= distances.size() || distances[target] == infinity) {
    return std::nullopt;
  }

  std::vector<std::size_t> path;
  for (std::size_t curr = target; curr != kInvalidVertex; curr = predecessors[curr]) {
    path.push_back(curr);
    if (curr == source) {
      break;
    }
  }

  // If the path did not connect back to our source, target was unreachable
  if (path.empty() || path.back() != source) {
    return std::nullopt;
  }

  std::ranges::reverse(path);
  return path;
}

} // namespace graph