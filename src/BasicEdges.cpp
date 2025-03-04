#include "BasicEdges.hpp"

#include <limits>

using namespace std;

// Constructor: Initializes the BasicEdges with a set of edges.
BasicEdges::BasicEdges(const vector<pair<int, int>> &initialEdges)
{
    for (const auto &edge : initialEdges)
    {
        edges.emplace_back(edge.first, edge.second, numeric_limits<double>::quiet_NaN());
    }
}

// adds edge to edges-vector
void BasicEdges::addEdge(int source, int destination)
{
    if (destination < source)
    {
        swap(source, destination);
    }

    edges.emplace_back(source, destination, numeric_limits<double>::quiet_NaN());
}

// Retrieves the neighbors of a given node.
vector<int> BasicEdges::getNeighbors(int nodeID)
{
    vector<int> neighbors;
    int start, dest;

    for (const auto &edge : edges)
    {
        start = get<0>(edge);
        dest = get<1>(edge);

        if (start == nodeID)
        {
            neighbors.push_back(dest);
        }
        else if (dest == nodeID)
        {
            neighbors.push_back(start);
        }
    }

    return neighbors;
}

// Checks if an edge exists between two nodes.
bool BasicEdges::isEdge(int source, int destination)
{
    if (destination < source)
    {
        swap(source, destination);
    }

    for (const auto &edge : edges)
    {
        if (get<0>(edge) == source && get<1>(edge) == destination)
        {
            return true; // edge is found
        }
    }

    return false; // Only return false after checking all edges
}

// Retrieves all edges in the graph.
vector<pair<int, int>> BasicEdges::getEdges() const
{
    vector<pair<int, int>> edgesWithoutWeights;

    for (const auto &edge : edges)
    {
        edgesWithoutWeights.emplace_back(get<0>(edge), get<1>(edge));
    }

    return edgesWithoutWeights;
}

// sets weight of an edge
void BasicEdges::setWeight(int source, int destination, double weight)
{
    if (destination < source)
    {
        swap(source, destination);
    }

    auto it = find_if(edges.begin(), edges.end(), [&](const tuple<int, int, double> &edge)
                           { return get<0>(edge) == source && get<1>(edge) == destination; });

    if (it != edges.end())
    {
        get<2>(*it) = weight;
    }
}

// gets weight of an edge in graph
double BasicEdges::getWeight(int source, int destination) const
{   
    if (destination < source)
    {
        swap(source, destination);
    }
    
    auto it = find_if(edges.begin(), edges.end(),
                           [&](const tuple<int, int, double> &edge)
                           { return get<0>(edge) == source && get<1>(edge) == destination; });

    if (it != edges.end())
    {
        return get<2>(*it);
    }

    return numeric_limits<double>::quiet_NaN(); // return standard value if not found
}

// Retrieves number of edges
int BasicEdges::size()
{
    return edges.size();
}
