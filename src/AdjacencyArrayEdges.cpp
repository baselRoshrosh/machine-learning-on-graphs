#include "AdjacencyArrayEdges.hpp"

#include <limits> 

AdjacencyArrayEdges::AdjacencyArrayEdges(const std::vector<std::pair<int, int>> &initialEdges)
{
    std::vector<std::vector<int>> adjacencyList;

    for (auto [source, dest] : initialEdges) {
        adjacencyList[source].emplace_back(dest);
    }

    adjacencyArray(initialEdges.size());
    adjacencyOffsets(adjacencyList.size());
    
    int currentOffset = 0;
    for (int i = 0; i < adjacencyList.size(); i++){
        adjacencyOffsets[i] = currentOffset;
        adjacencyA
    }
}

void addEdge(int source, int destination)
{
}

std::vector<int> AdjacencyArrayEdges::getNeighbors(int nodeID)
{
    std::vector<int> neighbors;

    for (int offset = adjacencyOffsets[nodeID]; offset < adjacencyOffsets[nodeID + 1]; offset++)
    {
        neighbors.push_back(adjacencyArray[offset]);
    }

    return neighbors;
}

bool AdjacencyArrayEdges::isEdge(int source, int destination)
{
    for (int offset = adjacencyOffsets[source]; offset < adjacencyOffsets[source + 1]; offset++)
    {
        if (adjacencyArray[offset] == destination)
        {
            return true;
        }
    }

    return false
}

std::vector<std::pair<int, int>> AdjacencyArrayEdges::getEdges() const
{
    std::vector<std::pair<int, int>> edgesVector;

    for (){}

    return edgesVector;
}

void AdjacencyArrayEdges::setWeight(int source, int destination, double weight)
{
    if (destination < source) {
        std::swap(source, destination);
    }

    weights[{source, destination}] = weight;
}

double AdjacencyArrayEdges::getWeight(int source, int destination) const {
    if (destination < source) {
        std::swap(source, destination);
    }
    auto it = weights.find({source, destination});
    if (it != weights.end()) {
        return it->second;
    }

    return std::numeric_limits<double>::quiet_NaN();  // Return NaN if the edge does not exist
}

int AdjacencyArrayEdges::size() {
    return adjacencyArray.size() / 2;
}