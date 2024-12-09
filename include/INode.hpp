#ifndef INODE_HPP
#define INODE_HPP

#include <vector>

/**
 * @class Node
 * @brief Represents a graph node.
 *
 * This class is used to store the features of a node and provide an interface
 * for accessing node-specific information, such as its ID and feature vector.
 */
template <typename T>
class INode
{
public:
    /**
     * @brief Constructs a new Node object.
     *
     * @param id The unique identifier for the node.
     * @param featureVector The feature vector of the node.
     * @param label The label of the node.
     */
    INode(int id, std::vector<T> featureVector, int label);

    /**
     * @brief Get the ID of the node.
     *
     * @return int The unique identifier of the node.
     */
    virtual int getId() const = 0;

    /**
     * @brief Get the feature vector of the node.
     *
     * @return std::vector<T> The feature vector associated with the node.
     */
    virtual std::vector<T> getFeatureVector() const = 0;

    /**
     * @brief Get the label of the node.
     *
     * @return int The label of the node.
     */
    virtual int getLabel() const = 0;

    // Virtual destructor to support polymorphism
    virtual ~INode() = default;

private:
    int id;                  ///< unique node identifier
    std::vector<T> features; ///< the feature vector of the nodes
    int label;               ///< the label of the node
};

#endif