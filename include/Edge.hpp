#ifndef EDGE_HPP
#define EDGE_HPP

/**
 * @class Edge
 * @brief Represents an edge in an undirected graph.
 *
 * This class models an undirected edge between two nodes in a graph. 
 * The nodes are ordered such that getsource() always returns the node 
 * with the lower ID, and getdestination() returns the node with the higher ID.
 */
class Edge
{
public:
    /**
     * @brief Constructs an edge between two nodes.
     *
     * The constructor ensures that the smaller node ID is stored as `source`
     * and the larger node ID as `destination`.
     *
     * @param source The ID of the first node.
     * @param destination The ID of the second node.
     */
    Edge(int source, int destination);

    /**
     * @brief Gets the first node (smaller ID).
     *
     * @return int The ID of the first node.
     */
    virtual int getsource() const = 0;

    /**
     * @brief Gets the second node (larger ID).
     * 
     * @return int The ID of the second node.
     */
    virtual int getdestination() const = 0;

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
    int source; ///< The ID of the smaller node. 
    int destination; ///< The ID of the larger node.
};

#endif
