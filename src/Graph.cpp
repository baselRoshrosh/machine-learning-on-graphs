#include <memory>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cctype>
#include <cstdlib>
#include <limits>

#include "Graph.hpp"

using namespace std;

/**
 * @brief Constructs a graph by parsing from txt files.
 *
 * @param nodesFile The file containing node information.
 * @param edgesFile The file containing edge information.
 */
Graph::Graph(const string &nodesFile, const string &edgesFile)
{
    // Read edge file
    vector<pair<int, int>> initialEdges; // Vector to hold all edges
    ifstream edgesFileStream(edgesFile);
    if (!edgesFileStream.is_open())
    {
        cerr << "Failed to open edge file!" << endl;
        return;
    }

    string line;
    while (getline(edgesFileStream, line))
    {
        istringstream iss(line);
        int source, destination;
        if (iss >> source >> destination)
        {
            // Collect edge by adding it to the vector
            initialEdges.emplace_back(source, destination);
        }
        else
        {
            cerr << "Invalid line in edge file: " << line << endl;
        }
    }
    edgesFileStream.close();

    // Construct AdjacencyArrayEdges using the complete edge set
    edges = make_unique<AdjacencyArrayEdges>(initialEdges);

    // read node file
    ifstream nodesFileStream(nodesFile);
    if (!nodesFileStream.is_open())
    {
        cerr << "Failed to open node file!" << endl;
        return;
    }

    while (getline(nodesFileStream, line))
    {
        istringstream iss(line);
        // Node ID parsing
        int nodeId;
        iss >> nodeId;
        // Features parsing
        string featuresString;
        getline(iss, featuresString); // jump to start of features

        // remove \t after nodeId
        if (!featuresString.empty() && featuresString[0] == '\t')
        {
            featuresString.erase(0, 1); // Remove the first character
        }

        vector<double> features;
        istringstream featuresStream(featuresString);

        // parsing feature value
        int label;
        string feature;
        while (getline(featuresStream, feature, ','))
        {
            if (feature[0] == ' ')
            {
                feature.erase(0, 1);
            }
            size_t tmp = feature.find('\t');
            if (tmp != string::npos)
            {
                label = stoi(feature.substr(tmp + 1));
                feature = feature.substr(0, tmp);
            }
            if (feature == "'#'" || feature == "#")
            {
                features.push_back(numeric_limits<double>::quiet_NaN()); // Replace missing feature
            }
            else
            {
                features.push_back(stod(feature)); // Convert to double
            }
        }

        // Add Node to the Graph
        nodes.emplace_back(nodeId, features, label);
    }
    nodesFileStream.close();
}

vector<int> Graph::getNodes() const
{
    vector<int> nodeIds;
    for (const auto &node : nodes)
    {
        nodeIds.push_back(node.getId());
    }
    return nodeIds;
}

vector<pair<int, int>> Graph::getEdges() const
{
    return edges->getEdges();
}

vector<int> Graph::getNeighbors(int nodeId) const
{
    return edges->getNeighbors(nodeId);
}

int Graph::getNodeCount() const
{
    return nodes.size();
}

int Graph::getEdgeCount() const
{
    auto nonConstEdges = const_cast<IEdges *>(this->edges.get());
    return nonConstEdges->size();
}

vector<double> Graph::getFeatureById(int nodeId) const
{
    for (const auto &node : nodes)
    {
        if (node.getId() == nodeId)
        {
            return node.getFeatureVector();
        }
    }
    return {};
}

void Graph::updateFeatureById(int nodeId, const vector<double> &newFeatures)
{
    for (auto &node : nodes)
    {
        if (node.getId() == nodeId)
        {
            node.setFeatureVector(newFeatures);
            return;
        }
    }
    throw invalid_argument("Node ID not found");
}

void Graph::setEdgeWeight(int source, int destination, double weight)
{
    edges->setWeight(source, destination, weight);
}

double Graph::getEdgeWeight(int source, int destination) const
{
    return edges->getWeight(source, destination);
}

int Graph::getLabelById(int nodeId)
{
    for (const auto &node : nodes)
    {
        if (node.getId() == nodeId)
        {
            return node.getLabel();
        }
    }
    return 0;
}