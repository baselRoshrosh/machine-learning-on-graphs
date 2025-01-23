#ifndef NODE_TPP
#define NODE_TPP
#include "../include/INode.hpp"

/**
 * @class Node
 * @brief Concrete implementation of the INode interface.
 */
class Node : public INode {
public:
    Node() = default;

    /**
     * @brief Constructs a new Node object.
     *
     * @param id The unique identifier for the node.
     * @param featureVector The feature vector of the node.
     * @param label The label of the node.
     */
    Node(int id, std::vector<double> featureVector, int label) {
        this->id = id;
        this->features = featureVector;
        this->label = label;
    };

    int getId() const override { return this->id; }
    std::vector<double> getFeatureVector() const override { return this->features; }
    int getLabel() const override { return this->label; }
    void setFeatureVector(const std::vector<double>& updatedFeatures) { this->features = updatedFeatures; }
};

#endif // NODE_TPP
