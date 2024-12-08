#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>
#include <string>

class Edge; // Forward Declaration

/**
 * @class Graph
 * @brief Represents an undirected, sparse graph.
 *
 * This class provides an interface for working with a graph structure.
 * It supports adding and querying nodes and edges, as well as parsing
 * graph data from files.
 */
class Graph
{
public:
    /**
     * @brief Constructs a graph by parsing from CSV files.
     *
     * @param nodesFile The file containing node information.
     * @param edgesFile The file containing edge information.
     */
    Graph(const std::string &nodesFile, const std::string &edgesFile);

    /**
     * @brief Retrieves all nodes in the graph.
     *
     * @return std::vector<int> A list of all node IDs.
     */
    virtual std::vector<int> getNodes() const = 0;

    /**
     * @brief Retrieves all edges in the graph.
     *
     * @return std::vector<Edge> A list of all edges.
     */
    virtual std::vector<Edge> getEdges() const = 0;

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
    virtual ~Graph() = default;
};

#endif