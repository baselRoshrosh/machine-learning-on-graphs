#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <string>
#include <Node.hpp>
#include <Edge.hpp>

class Edge; // Forward Declaration

/**
 * @class Graph
 * @brief Represents an undirected, sparse graph.
 *
 * This class provides an interface for working with a graph structure.
 * It supports adding and querying nodes and edges, as well as parsing
 * graph data from files.
 */
template <typename T>
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

private:
    std::vector<T> nodes; ///< the feature vector of the nodes
    std::vector<T> edges; ///< the feature vector of the edges
};

template <typename T>
class Graph : public Graph<T>
{
public:
    Graph(const std::string &nodesFile, const std::string &edgesFile);

    std::vector<int> getNodes() const override
    {
        std::vector<int> nodeIds;
        for (const auto &node : nodes)
        {
            nodeIds.push_back(node.getId());
        }
        return nodeIds;
    }

    std::vector<Edge> getEdges() const override
    {
        return edges;
    }

    std::vector<int> getNeighbors(int nodeId) const override
    {
        std::vector<int> neighbors;
        for (const auto &edge : edges)
        {
            if (edge.getSource() == nodeId)
            {
                neighbors.push_back(edge.getTarget());
            }
            else if (edge.getTarget() == nodeId)
            {
                neighbors.push_back(edge.getSource());
            }
        }
        return neighbors;
    }

    int getNodeCount() const override
    {
        return nodes.size();
    }

    int getEdgeCount() const override
    {
        return edges.size();
    }

private:
    std::vector<Node> nodes;
    std::vector<Edge> edges;
};

#endif