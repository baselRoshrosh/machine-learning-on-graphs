#ifndef IEDGES_HPP
#define IEDGES_HPP

#include <vector>
#include <utility>

/**
 * @class IEdges
 * @brief Abstract interface for managing graph edges.
 *
 * This interface defines the basic methods for working with edges in a graph,
 * including retrieving neighbors, checking for edge existence, and accessing all edges.
 */
class IEdges
{
public:
    /**
     * @brief Retrieves the neighbors of a given node.
     *
     * This function returns a list of all nodes directly connected to the specified node
     * by an edge.
     *
     * @param nodeID The ID of the node whose neighbors are to be retrieved.
     * @return std::vector<int> A list of neighboring node IDs.
     */
    virtual std::vector<int> getNeighbors(int nodeID) = 0;

    /**
     * @brief Checks if an edge exists between two nodes.
     *
     * This function checks whether an edge exists between the source and destination nodes.
     *
     * @param source The ID of the source node.
     * @param destination The ID of the destination node.
     * @return bool True if the edge exists, otherwise false.
     */
    virtual bool isEdge(int source, int destination) = 0;

    /**
     * @brief Retrieves all edges in the graph.
     *
     * This function returns a vector containing all edges represented as pairs of node IDs.
     *
     * @return std::vector<std::pair<int, int>> A list of all edges as node ID pairs.
     */
    virtual std::vector<std::pair<int, int>> getEdges() const = 0;

    /**
     * @brief Returns number of stored edges
     *
     * @return int Number of stored edges
     */
    virtual int size() = 0;

    /**
     * @brief Adds a new edge to the edge list.
     *
     * @param source The ID of the source node.
     * @param destination The ID of the destination node.
     */
    virtual void addEdge(int source, int destination) = 0;

    /**
     * Allows to set a weight of a specified edge
     *
     * @param source The ID of the source node.
     * @param destination The ID of the destination node.
     */
    virtual void setWeight(int source, int destination, double weight) = 0;

    /**
     * Gets the weight of a specified edge.
     *
     * @param source The ID of the source node.
     * @param destination The ID of the destination node.
     * @return the Weight of the specified edge
     */
    virtual double getWeight(int source, int destination) const = 0;

    // Virtual destructor to ensure proper cleanup of derived classes
    virtual ~IEdges() = default;
};

#endif