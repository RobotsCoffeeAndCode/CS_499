#include "Dijkstra.hpp"
#include "ContinentGraph.hpp"

#include <algorithm>
#include <queue>
#include <stdexcept>

namespace graph {

// Internal struct for priority queue ordering
struct QueueNode {
  int distance;
  std::size_t vertex;

  // Greater-than operator creates a min-heap via std::greater
  bool operator>(const QueueNode &other) const noexcept {
    return distance > other.distance;
  }
};

DijkstraResult dijkstra(const ContinentGraph &graph, std::size_t startVertex) {
  const std::size_t numOfVertices = graph.getVertexCount();
  if (startVertex >= numOfVertices) {
    throw std::out_of_range("Start vertex index out of bounds.");
  }

  std::vector<int> distances(numOfVertices, DijkstraResult::infinity);
  // Predecessor of a node initialized to itself
  std::vector<std::size_t> predecessors(numOfVertices);
  for (std::size_t i = 0; i < numOfVertices; ++i) {
    predecessors[i] = i;
  }

  // Min-heap storing {distance, vertex}
  std::priority_queue<QueueNode, std::vector<QueueNode>,
                      std::greater<QueueNode>>
      prioq;

  distances[startVertex] = 0;
  prioq.push(QueueNode{.distance = 0, .vertex = startVertex});

  while (!prioq.empty()) {
    const auto [currentDist, u] = prioq.top();
    prioq.pop();

    // Stale entry check: a shorter path to u was already processed
    if (currentDist > distances[u]) {
      continue;
    }

    for (const Edge &edge : graph.getNeighbors(u)) {
      if (edge.weight < 0) {
        throw std::invalid_argument(
            "Dijkstra's algorithm does not support negative weights.");
      }

      // Check against overflow before relaxing
      if (distances[u] + edge.weight < distances[edge.to]) {
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

std::optional<std::vector<std::size_t>>
DijkstraResult::getPathTo(std::size_t target) const {
  if (target >= distances.size() || distances[target] == infinity) {
    return std::nullopt; // Target is unreachable or invalid
  }

  std::vector<std::size_t> path;
  for (std::size_t curr = target; curr != source; curr = predecessors[curr]) {
    path.push_back(curr);
  }
  path.push_back(source);

  std::ranges::reverse(path.begin(), path.end());
  return path;
}

} // namespace graph