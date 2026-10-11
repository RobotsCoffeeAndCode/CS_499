// ContinentGraph.cpp
#include "ContinentGraph.hpp"
#include "RescueUtils.hpp"

#include <iostream>
#include <stdexcept>

namespace graph {

ContinentGraph::ContinentGraph(std::size_t vertices)
    : numVertices_(vertices), adjList_(vertices) {}

void ContinentGraph::addEdge(VertexId start, VertexId end, int weight) {
  if (start >= numVertices_ || end >= numVertices_) {
    throw std::out_of_range("Vertex index out of bounds.");
  }

  adjList_[start].push_back(Edge{.to = end, .weight = weight});

  // Make the edge bidirectional if it's not a self-loop
  if (start != end) {
    adjList_[end].push_back(Edge{.to = start, .weight = weight});
  }
}

const std::vector<Edge> &
ContinentGraph::getNeighbors(VertexId currentVertex) const {
  if (currentVertex >= numVertices_) {
    throw std::out_of_range("Vertex index out of bounds.");
  }
  return adjList_[currentVertex];
}

std::size_t ContinentGraph::getVertexCount() const noexcept {
  return numVertices_;
}

// Prints the graph using human-readable continent names from RescueUtils
void ContinentGraph::printGraph() const {
  for (std::size_t u = 0; u < numVertices_; ++u) {
    std::cout << "From " << RescueUtils::decodeContinentValue(u) << ":\n";
    for (const auto &edge : adjList_[u]) {
      std::cout << "  -> " << RescueUtils::decodeContinentValue(edge.to)
                << "  $" << edge.weight << "\n";
    }
  }
}

} // namespace graph