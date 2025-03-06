#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <memory>
#include <vector>
#include <string>
#include <utility>

#include "interfaces/IEdges.hpp"
#include "AdjacencyArrayEdges.hpp"
#include "Node.hpp"

using namespace std;

/**
 * @class Graph
 * @brief Represents an undirected, sparse graph.
 *
 * This class provides an interface for working with a graph structure.
 * It supports adding and querying nodes and edges, as well as parsing
 * graph data from files.
 */
class Graph
{
private:
    vector<Node> nodes;           /// the vector of nodes
    unique_ptr<IEdges> edges;   /// object holding the pool of edges (abstract interface)
public:
    /**
     * @brief Constructs a graph by parsing from txt files.
     *
     * @param nodesFile The file containing node information.
     * @param edgesFile The file containing edge information.
     */
    Graph(const string &nodesFile, const string &edgesFile);

    /**
     * @brief Retrieves all nodes in the graph.
     *
     * @return vector<int> A list of all node IDs.
     */
    vector<int> getNodes() const;

    /**
     * @brief Retrieves all edges in the graph.
     *
     * @return vector<pair<int, int>> A list of all edges.
     */
    vector<pair<int, int>> getEdges() const;

    /**
     * @brief Retrieves neighbors of a specified node.
     *
     * @return vector<int> A list of all neighbor node IDs.
     */
    vector<int> getNeighbors(int nodeId) const;

    /**
     * @brief Retrieves node count of graph.
     *
     * @return int Number of all the nodes in the graph.
     */
    int getNodeCount() const;

    /**
     * @brief Retrieves edge count of graph.
     *
     * @return int Number of all the edges in the graph.
     */
    int getEdgeCount() const;

    /**
     * @brief Retrieves the feature vector of a node by its ID.
     *
     * @param nodeId The ID of the node.
     * @return vector<double> The feature vector of the node or an empty vector if the node is not found.
     */
    vector<double> getFeatureById(int nodeId) const;

    /**
     * @brief Update the entire feature vector of a node by its ID.
     * @param nodeId The ID of the node whose feature vector is to be updated.
     * @param newFeatures The new feature vector to set for the node.
     */
    void updateFeatureById(int nodeId, const vector<double> &newFeatures);

    /**
     * Allows to set a weight of a specified edge
     *
     * @param source The ID of the source node.
     * @param destination The ID of the destination node.
     */
    void setEdgeWeight(int source, int destination, double weight);

    /**
     * Gets the weight of a specified edge.
     *
     * @param source The ID of the source node.
     * @param destination The ID of the destination node.
     * @return the Weight of the specified edge
     */
    double getEdgeWeight(int source, int destination) const;

    /**
     * @brief Retrieves the label associated with a given node ID.
     *
     * This function takes a node ID as input and returns the corresponding label.
     * If the node ID does not exist, the behavior is undefined.
     *
     * @param nodeId The ID of the node whose label is to be retrieved.
     * @return The label associated with the specified node ID.
     */
    int getLabelById(int nodeId);
};

#endif