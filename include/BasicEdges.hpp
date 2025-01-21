#ifndef BASICEDGES_HPP
#define BASICEDGES_HPP

#include <vector>
#include <utility>

/**
 * @class IBasicEdges
 *
 * This class stores and manages graph edges using a simple
 * `std::vector<std::tuple<int, int, int>>` data structure. It provides methods
 * for retrieving neighbors, checking edge existence, and accessing all edges.
 */
class IBasicEdges
{
public:
    /**
     * @brief Default constructor for INode.
     */
    IBasicEdges() = default;

    /**
     * @brief Constructor to initialize edges with a predefined set of edges.
     *
     * @param initialEdges A vector of edges to initialize the graph.
     */
    IBasicEdges(const std::vector<std::tuple<int, int, int>> &initialEdges);

    /**
     * @brief Adds a new edge to the edge list.
     *
     * @param source The ID of the source node.
     * @param destination The ID of the destination node.
     */
    void addEdge(int source, int destination) const;

    /**
     * @brief Retrieves the neighbors of a given node.
     *
     * @param nodeID The ID of the node whose neighbors are to be retrieved.
     * @return std::vector<int> A list of neighboring node IDs.
     */
    std::vector<int> getNeighbors(int nodeID) const;

    /**
     * @brief Checks if an edge exists between two nodes.
     *
     * @param source The ID of the source node.
     * @param destination The ID of the destination node.
     * @return bool True if the edge exists, otherwise false.
     */
    bool isEdge(int source, int destination) const;

    /**
     * @brief Retrieves all edges in the graph.
     *
     * @return std::vector<std::pair<int, int>> A list of all edges as node ID pairs.
     */
    std::vector<std::tuple<int, int, int>> getEdges() const;

    /**
     * @brief Returns number of stored edges
     *
     * @return int Number of stored edges
     */
    int getSize() const;

    /**
     * @brief Constructs and returns the adjacency matrix representation of the graph.
     *
     * @param numNodes The number of nodes in the graph.
     * @return std::vector<std::vector<int>> The adjacency matrix.
     */
    virtual std::vector<std::vector<int>> getMatrix(int numNodes) const = 0;

    // Virtual destructor to support polymorphism
    virtual ~IBasicEdges() = default;

private:
    std::vector<std::pair<int, int>> edges; ///< Stores all edges as pairs of node IDs.
};

#endif