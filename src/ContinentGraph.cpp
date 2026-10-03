#include "ContinentGraph.hpp"

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

void ContinentGraph::printGraph() const {
  for (std::size_t index = 0; index < numVertices_; ++index) {
    switch (index) {
    case 0:
      std::cout << "From Africa" << ":\n";
      break;
    case 1:
      std::cout << "From Antarctica" << ":\n";
      break;
    case 2:
      std::cout << "From Asia" << ":\n";
      break;
    case 3:
      std::cout << "From Australia" << ":\n";
      break;
    case 4:
      std::cout << "From Europe" << ":\n";
      break;
    case 5:
      std::cout << "From North America" << ":\n";
      break;
    case 6:
      std::cout << "From South America" << ":\n";
      break;
    default:
      std::cout << "From Unknown" << ":\n";
      break;
    }
    for (const auto &edge : adjList_[index]) {
      switch (edge.to) {
      case 0:
        std::cout << "  -> " << "Africa" << "  $" << edge.weight
                  << "\n";
        break;
      case 1:
        std::cout << "  -> " << "Antarctica" << " $" << edge.weight
                  << "\n";
        break;
      case 2:
        std::cout << "  -> " << "Asia" << " $" << edge.weight << "\n";
        break;
      case 3:
        std::cout << "  -> " << "Australia" << " $" << edge.weight
                  << "\n";
        break;
      case 4:
        std::cout << "  -> " << "Europe" << " $" << edge.weight
                  << "\n";
        break;
      case 5:
        std::cout << "  -> " << "North America" << " $" << edge.weight
                  << "\n";
        break;
      case 6:
        std::cout << "  -> " << "South America" << " $" << edge.weight
                  << "\n";
        break;
      default:
        std::cout << "  -> " << "unknown" << " $" << edge.weight
                  << "\n";
        break;
      }
    }
  }
}

} // namespace graph