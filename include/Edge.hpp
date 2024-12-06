#ifndef EDGE_HPP
#define EDGE_HPP

/**
 * @class Edge
 * @brief Represents an edge in an undirected graph.
 *
 * This class models an undirected edge between two nodes in a graph. 
 * The nodes are ordered such that getNode1() always returns the node 
 * with the lower ID, and getNode2() returns the node with the higher ID.
 */
class Edge
{
public:
    /**
     * @brief Constructs an edge between two nodes.
     *
     * The constructor ensures that the smaller node ID is stored as `node1`
     * and the larger node ID as `node2`.
     *
     * @param node1 The ID of the first node.
     * @param node2 The ID of the second node.
     */
    Edge(int node1, int node2);

    /**
     * @brief Gets the first node (smaller ID).
     *
     * @return int The ID of the first node.
     */
    virtual int getNode1() const = 0;

    /**
     * @brief Gets the second node (larger ID).
     * 
     * @return int The ID of the second node.
     */
    virtual int getNode2() const = 0;

    /**
     * @brief Compares equality between two edges.
     * 
     * Two edges are considered equal if they connect the same nodes, 
     * regardless of order (undirected graph).
     *
     * @param other The edge to compare with.
     * @return true If the edges are equal.
     * @return false Otherwise.
     */
    bool operator==(const Edge& other) const;

    // Virtual destructor to support polymorphism
    virtual ~Edge() = default;

private:
    int node1; ///< The ID of the smaller node. 
    int node2; ///< The ID of the larger node.
};

#endif
