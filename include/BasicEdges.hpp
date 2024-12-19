#ifndef BASICEDGES_HPP
#define BASICEDGES_HPP

#include "IEdges.hpp"
#include <vector>
#include <algorithm>

/**
 * @class BasicEdges
 * @brief A concrete implementation of the IEdges interface.
 *
 * This class stores and manages graph edges using a simple 
 * `std::vector<std::pair<int, int>>` data structure. It provides methods 
 * for retrieving neighbors, checking edge existence, and accessing all edges.
 */
class BasicEdges : public IEdges
{
public:
    /**
     * @brief Default constructor for BasicEdges.
     */
    BasicEdges() = default;

    /**
     * @brief Constructor to initialize edges with a predefined set of edges.
     *
     * @param initialEdges A vector of edges to initialize the graph.
     */
    BasicEdges(const std::vector<std::pair<int, int>>& initialEdges);

    /**
     * @brief Adds a new edge to the edge list.
     *
     * @param source The ID of the source node.
     * @param destination The ID of the destination node.
     */
    void addEdge(int source, int destination);

    /**
     * @brief Retrieves the neighbors of a given node.
     *
     * @param nodeID The ID of the node whose neighbors are to be retrieved.
     * @return std::vector<int> A list of neighboring node IDs.
     */
    std::vector<int> getNeighbors(int nodeID) override;

    /**
     * @brief Checks if an edge exists between two nodes.
     *
     * @param source The ID of the source node.
     * @param destination The ID of the destination node.
     * @return bool True if the edge exists, otherwise false.
     */
    bool isEdge(int source, int destination) override;

    /**
     * @brief Retrieves all edges in the graph.
     *
     * @return std::vector<std::pair<int, int>> A list of all edges as node ID pairs.
     */
    std::vector<std::pair<int, int>> getEdges() const override;

private:
    std::vector<std::pair<int, int>> edges; ///< Stores all edges as pairs of node IDs.
};

#endif