#include "AdjacencyArrayEdges.hpp"

#include <limits>
#include <algorithm>

/*
 * =========== constructors ===============
 */
AdjacencyArrayEdges::~AdjacencyArrayEdges() = default;

AdjacencyArrayEdges::AdjacencyArrayEdges(const std::vector<std::pair<int, int>> &initialEdges)
{
    // 1: create adjacency list
    std::unordered_map<int, std::vector<int>> adjacencyList;
    for (auto [source, dest] : initialEdges)
    {
        if (dest < source)
            std::swap(source, dest);
        adjacencyList[source].push_back(dest);
        adjacencyList[dest].push_back(source);
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
 void AdjacencyArrayEdges::fillAdjacencyArrayFromList(const std::unordered_map<int, std::vector<int>> &adjacencyList)
 {
     // Determine the maximum node id from both keys and neighbor values.
     int maxNode = 0;
     for (const auto &entry : adjacencyList)
     {
         maxNode = std::max(maxNode, entry.first);
         for (int neighbor : entry.second)
         {
             maxNode = std::max(maxNode, neighbor);
         }
     }
 
     // Create a vector of vectors (one for each node id from 0 to maxNode)
     std::vector<std::vector<int>> fullAdjList(maxNode + 1);
     
     // Fill fullAdjList using the provided adjacencyList; nodes not in the map remain empty.
     for (const auto &entry : adjacencyList)
     {
         fullAdjList[entry.first] = entry.second;
     }
 
     // Calculate total number of adjacency entries.
     int totalAdj = 0;
     for (const auto &neighbors : fullAdjList)
     {
         totalAdj += neighbors.size();
     }
 
     // Resize the flat array and offsets vector.
     adjacencyArray.resize(totalAdj);
     // Use offsets size = number of nodes + 1 (last element holds the total count)
     adjacencyOffsets.resize(fullAdjList.size() + 1, 0);
 
     int offset = 0;
     for (size_t i = 0; i < fullAdjList.size(); i++)
     {
         adjacencyOffsets[i] = offset;
         // Optionally sort each node's neighbor list for consistency.
         std::sort(fullAdjList[i].begin(), fullAdjList[i].end());
         for (int neighbor : fullAdjList[i])
         {
             adjacencyArray[offset++] = neighbor;
         }
     }
     adjacencyOffsets[fullAdjList.size()] = totalAdj; // End marker
 }
 