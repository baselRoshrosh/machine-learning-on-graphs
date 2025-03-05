#include "AdjacencyArrayEdges.hpp"

#include <limits>

/*
 * =========== constructors ===============
 */

AdjacencyArrayEdges::AdjacencyArrayEdges(const std::vector<std::pair<int, int>> &initialEdges)
{
    // 1: create adjacency list
    std::unordered_map<int, std::vector<int>> adjacencyList;
    for (auto [source, dest] : initialEdges)
    {
        adjacencyList[source].emplace_back(dest);
    }

    fillAdjacencyArrayFromList(adjacencyList);
}

/*
 * ======= Interface Methoden ===============
 */

void AdjacencyArrayEdges::addEdge(int source, int destination)
{
    if (isEdge(source, destination))
    {
        return;
    }

    adjacencyArray.resize(adjacencyArray.size() + 2);

    // add source->dest
    adjacencyArray.insert(adjacencyArray.begin() + adjacencyOffsets[source], destination);

    if (source + 1 < adjacencyArray.size())
    {
        for (int i = source + 1; i < adjacencyArray.size(); i++)
        {
            ++adjacencyOffsets[i];
        }
    }

    // add dest->source
    adjacencyArray.insert(adjacencyArray.begin() + adjacencyOffsets[destination], source);

    if (destination + 1 < adjacencyArray.size())
    {
        for (int i = destination + 1; i < adjacencyArray.size(); i++)
        {
            ++adjacencyOffsets[i];
        }
    }
}

std::vector<int> AdjacencyArrayEdges::getNeighbors(int nodeID)
{
    std::vector<int> neighbors;

    if (nodeID >= adjacencyOffsets.size())
    {
        return neighbors; // Return empty vector if nodeID is invalid
    }

    int startAdjacents = adjacencyOffsets[nodeID];
    int endAdjacents = (nodeID + 1 < adjacencyOffsets.size()) ? adjacencyOffsets[nodeID + 1] : adjacencyArray.size();

    for (int offset = startAdjacents; offset < endAdjacents; offset++)
    {
        neighbors.push_back(adjacencyArray[offset]);
    }

    return neighbors;
}

bool AdjacencyArrayEdges::isEdge(int source, int destination)
{
    if (source >= adjacencyOffsets.size())
    {
        return false; // Source node is invalid
    }

    // Define start and end indices correctly
    int startAdjacents = adjacencyOffsets[source];
    int endAdjacents = (source + 1 < adjacencyOffsets.size()) ? adjacencyOffsets[source + 1] : adjacencyArray.size();

    for (int offset = startAdjacents; offset < endAdjacents; offset++)
    {
        if (adjacencyArray[offset] == destination)
        {
            return true;
        }
    }

    return false;
}

std::vector<std::pair<int, int>> AdjacencyArrayEdges::getEdges() const
{
    std::vector<std::pair<int, int>> edgesVector;

    for (int currentNode = 0; currentNode < adjacencyOffsets.size(); currentNode++)
    {
        int startAdjacents = adjacencyOffsets[currentNode];
        int endAdjacents = (currentNode + 1 < adjacencyOffsets.size()) ? adjacencyOffsets[currentNode + 1] : adjacencyArray.size();

        for (int offset = startAdjacents; offset < endAdjacents; offset++)
        {
            int currentNodeNeighbor = adjacencyArray[offset];

            // no edge duplicates
            if (currentNode < currentNodeNeighbor)
            {
                edgesVector.emplace_back(currentNode, adjacencyArray[offset]); // adjacencyArray[offset] iterates through currentNode's neighbors
            }
        }
    }

    return edgesVector;
}

void AdjacencyArrayEdges::setWeight(int source, int destination, double weight)
{
    if (destination < source)
    {
        std::swap(source, destination);
    }

    weights[{source, destination}] = weight;
}

double AdjacencyArrayEdges::getWeight(int source, int destination) const
{
    if (destination < source)
    {
        std::swap(source, destination);
    }
    auto it = weights.find({source, destination});
    if (it != weights.end())
    {
        return it->second;
    }

    return std::numeric_limits<double>::quiet_NaN(); // Return NaN if the edge does not exist
}

int AdjacencyArrayEdges::size()
{
    return adjacencyArray.size() / 2;
}

/*
 * ========= helper methods ============
 */

void AdjacencyArrayEdges::fillAdjacencyArrayFromList(std::unordered_map<int, std::vector<int>> adjacencyList)
{
    int numAdjacents = 0;
    for (auto &[node, adjacents] : adjacencyList)
    {
        numAdjacents += adjacents.size();
    }

    adjacencyArray.resize(numAdjacents);
    adjacencyOffsets.resize(adjacencyList.size());

    int currentOffset = 0;
    for (int i = 0; i < adjacencyList.size(); i++)
    {
        adjacencyOffsets[i] = currentOffset;
        auto &adjacents = adjacencyList[i];

        for (int j = 0; j < adjacents.size(); j++)
        {
            adjacencyArray[currentOffset + j] = adjacents[j];
        }
        currentOffset += adjacents.size(); // setting offset to new nodes
    }
}