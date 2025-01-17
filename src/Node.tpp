#ifndef NODE_TPP
#define NODE_TPP
#include "../include/INode.hpp"

/**
 * @class Node
 * @brief Concrete implementation of the INode interface.
 */
template <typename T>
class Node : public INode<T>
{
public:
    Node() = default;
    /**
     * @brief Constructs a new Node object.
     *
     * @param id The unique identifier for the node.
     * @param featureVector The feature vector of the node.
     * @param label The label of the node.
     */
    Node(int id, std::vector<T> featureVector, int label)
    {
        this->id = id;
        this->features = featureVector;
        this->label = label;
    };

    int getId() const override { return this->id; }
    std::vector<T> getFeatureVector() const override { return this->features; }
    int getLabel() const override { return this->label; }
    void setFeatureVector(const std::vector<T>& updatedFeatures) { this->features = updatedFeatures; }

    
};
#endif // NODE_TPP