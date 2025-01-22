#ifndef BASICEDGES_HPP
#define BASICEDGES_HPP

#include "IEdges.hpp"
#include <vector>
#include <utility>

/**
 * @class IBasicEdges
 *
 * This class stores and manages graph edges using a simple
 * `std::vector<std::tuple<int, int, int>>` data structure. It provides methods
 * for retrieving neighbors, checking edge existence, and accessing all edges.
 */
class IComplexEdges : public IEdges
{
public:
    /**
     * @brief Default constructor for INode.
     */
    IComplexEdges() = default;

    /**
     * @brief Constructs and returns the adjacency matrix representation of the graph.
     *
     * @param numNodes The number of nodes in the graph.
     * @return std::vector<std::vector<int>> The adjacency matrix.
     */
    virtual std::vector<std::vector<int>> getAdjacencyMatrix(int numNodes) const = 0;

private:
    std::vector<std::pair<int, int>> edges; ///< Stores all edges as pairs of node IDs.
};

#endif