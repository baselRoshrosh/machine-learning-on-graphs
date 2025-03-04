#ifndef BASICEDGES_HPP
#define BASICEDGES_HPP

#include <vector>
#include <tuple>
#include <algorithm>

#include "interfaces/IEdges.hpp"

using namespace std;

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
    BasicEdges(const vector<pair<int, int>> &initialEdges);

    /**
     * @brief Adds a new edge to the edge list.
     *
     * @param source The ID of the source node.
     * @param destination The ID of the destination node.
     */
    void addEdge(int source, int destination) override;

    /**
     * @brief Retrieves the neighbors of a given node.
     *
     * @param nodeID The ID of the node whose neighbors are to be retrieved.
     * @return vector<int> A list of neighboring node IDs.
     */
    vector<int> getNeighbors(int nodeID) override;

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
     * @return vector<pair<int, int>> A list of all edges as node ID pairs.
     */
    vector<pair<int, int>> getEdges() const override;

    /**
     * Allows to set a weight of a specified edge
     *
     * @param source The ID of the source node.
     * @param destination The ID of the destination node.
     */
    void setWeight(int source, int destination, double weight) override;

    /**
     * Gets the weight of a specified edge.
     *
     * @param source The ID of the source node.
     * @param destination The ID of the destination node.
     * @return the Weight of the specified edge
     */
    double getWeight(int source, int destination) const override;

    /**
     * @brief Returns number of stored edges
     *
     * @return int Number of stored edges
     */
    int size() override;

private:
    vector<tuple<int, int, double>> edges; ///< Stores all edges as pairs of node IDs.
};

#endif