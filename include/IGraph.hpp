#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <memory>
#include <vector>
#include <string>
#include <utility>
#include "IEdges.hpp"
#include "BasicEdges.hpp"
#include "INode.hpp"



/**
 * @class Graph
 * @brief Represents an undirected, sparse graph.
 *
 * This class provides an interface for working with a graph structure.
 * It supports adding and querying nodes and edges, as well as parsing
 * graph data from files.
 */
template <typename T>
class IGraph
{
public:
    /**
     * @brief Retrieves all nodes in the graph.
     *
     * @return std::vector<int> A list of all node IDs.
     */
    virtual std::vector<int> getNodes() const = 0;

    /**
     * @brief Retrieves all edges in the graph.
     *
     * @return std::vector<std::pair<int, int>> A list of all edges.
     */
    virtual std::vector<std::pair<int, int>> getEdges() const = 0;

    /**
     * @brief Retrieves neighbors of a specified node.
     *
     * @return std::vector<int> A list of all neighbor node IDs.
     */
    virtual std::vector<int> getNeighbors(int nodeId) const = 0;

    /**
     * @brief Retrieves node count of graph.
     *
     * @return int Number of all the nodes in the graph.
     */
    virtual int getNodeCount() const = 0;

    /**
     * @brief Retrieves edge count of graph.
     *
     * @return int Number of all the edges in the graph.
     */
    virtual int getEdgeCount() const = 0;

    // Virtual destructor to support polymorphism
    virtual ~IGraph() = default;

private:
    std::vector<T> nodes; /// the vector of nodes
    std::unique_ptr<IEdges> edges; /// object holding the pool of edges
};

#include "Graph.tpp"

#endif