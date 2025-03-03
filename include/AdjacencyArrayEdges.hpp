#ifndef ADJACENCY_ARRAY_EDGES_HPP
#define ADJACENCY_ARRAY_EDGES_HPP

#include "interfaces/IEdges.hpp"

#include <unordered_map>

// custom defined hash for pairs as they are not naturally supported
struct pair_hash
{
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2> &p) const
    {
        return std::hash<T1>()(p.first) ^ (std::hash<T2>()(p.second) << 1);
    }
};

class AdjacencyArrayEdges : public IEdges
{
public:
    /**
     * Default Constructor for AdjacencyArrayEdges
     */
    AdjacencyArrayEdges() = default;

    /**
     * Constructor from a List of Edges
     *
     * @param initialEdges Pointer to the List of Edges
     */
    AdjacencyArrayEdges(const std::vector<std::pair<int, int>> &initialEdges);

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

protected:
    std::vector<int> adjacencyOffsets;                                         ///< tracks beginning of adjacency list of node in adjacencyArray
    std::vector<int> adjacencyArray;                                           ///< concatenated adjacency lists
    std::unordered_map<std::pair<int, int>, double, struct pair_hash> weights; ///< keeps track of edge weights

    void fillAdjacencyArrayFromList(std::unordered_map<int, std::vector<int>> adjacencyList);
};

#endif