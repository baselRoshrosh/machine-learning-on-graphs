#include "../include/BasicEdges.hpp"

// Constructor: Initializes the BasicEdges with a set of edges.
BasicEdges::BasicEdges(const std::vector<std::pair<int, int>> &initialEdges)
    : edges(initialEdges) {}

// adds edge to edges-vector
void BasicEdges::addEdge(int source, int destination)
{
    edges.emplace_back(source, destination);
}

// Retrieves the neighbors of a given node.
std::vector<int> BasicEdges::getNeighbors(int nodeID)
{
    std::vector<int> neighbors;

    for (const auto &edge : edges)
    {
        if (edge.first == nodeID)
        {
            neighbors.push_back(edge.second);
        }
    }

    return neighbors;
}

// Checks if an edge exists between two nodes.
bool BasicEdges::isEdge(int source, int destination)
{
    for (const auto &edge : edges)
    {
        // for undirected graphs
        if (edge.first == source && edge.second == destination || edge.second == source && edge.first == destination)
        {
            return true;
        }

        return false; // edge was not found
    }
}

// Retrieves all edges in the graph.
std::vector<std::pair<int, int>> BasicEdges::getEdges() const
{
    return edges;
}

// Retrieves number of edges
int BasicEdges::size()
{
    return edges.size();
}