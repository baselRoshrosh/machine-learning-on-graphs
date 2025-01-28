#ifndef INODE_HPP
#define INODE_HPP

#include <vector>

/**
 * @class Node
 * @brief Represents a graph node with features and labels.
 *
 * This class stores information about a node, including its ID, feature vector, and label.
 */
class Node {
public:
    /**
     * @brief Default constructor for Node.
     */
    Node() = default;

    /**
     * @brief Constructs a new Node object.
     *
     * @param id The unique identifier for the node.
     * @param featureVector The feature vector of the node.
     * @param label The label of the node.
     */
    Node(int id, const std::vector<double>& featureVector, int label);

    /**
     * @brief Get the ID of the node.
     *
     * @return int The unique identifier of the node.
     */
    int getId() const;

    /**
     * @brief Get the feature vector of the node.
     *
     * @return std::vector<double> The feature vector associated with the node.
     */
    std::vector<double> getFeatureVector() const;

    /**
     * @brief Sets the feature vector.
     *
     * @param updatedFeatures The new feature vector to be set.
     */
    void setFeatureVector(const std::vector<double>& updatedFeatures);

    /**
     * @brief Get the label of the node.
     *
     * @return int The label of the node.
     */
    int getLabel() const;


protected:
    int id;                  ///< unique node identifier
    std::vector<double> features; ///< the feature vector of the nodes
    int label;               ///< the label of the node
};


#endif
