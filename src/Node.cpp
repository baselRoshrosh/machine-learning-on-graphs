#include "../include/Node.hpp"

using namespace std;

/**
 * @brief Constructs a new Node object.
 *
 * @param id The unique identifier for the node.
 * @param featureVector The feature vector of the node.
 * @param label The label of the node.
 */
Node::Node(int id, const vector<double> &featureVector, int label)
    : id(id), features(featureVector), label(label) {}

/**
 * @brief Get the ID of the node.
 *
 * @return int The unique identifier of the node.
 */
int Node::getId() const
{
    return id;
}

/**
 * @brief Get the feature vector of the node.
 *
 * @return vector<double> The feature vector associated with the node.
 */
vector<double> Node::getFeatureVector() const
{
    return features;
}

/**
 * @brief Sets the feature vector.
 *
 * @param updatedFeatures The new feature vector to be set.
 */
void Node::setFeatureVector(const vector<double> &updatedFeatures)
{
    features = updatedFeatures;
}

/**
 * @brief Get the label of the node.
 *
 * @return int The label of the node.
 */
int Node::getLabel() const
{
    return label;
}
