#include <memory>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cctype>
#include <cstdlib>
#include <limits>

#include "Graph.hpp"

/**
 * @brief Constructs a graph by parsing from txt files.
 *
 * @param nodesFile The file containing node information.
 * @param edgesFile The file containing edge information.
 */
Graph::Graph(const std::string &nodesFile, const std::string &edgesFile) : edges(std::make_unique<BasicEdges>())
{
    // Initialize the edges using BasicEdges

    // Read edge file
    std::ifstream edgesFileStream(edgesFile);
    if (!edgesFileStream.is_open())
    {
        std::cerr << "Failed to open edge file!" << std::endl;
        return;
    }

    std::string line;
    while (std::getline(edgesFileStream, line))
    {
        std::istringstream iss(line);
        int source, destination;
        if (iss >> source >> destination)
        {
            // Add edge using BasicEdges' addEdge method
            edges->addEdge(source, destination);
        }
        else
        {
            std::cerr << "Invalid line in edge file: " << line << std::endl;
        }
    }
    edgesFileStream.close();

    // read node file
    std::ifstream nodesFileStream(nodesFile);
    if (!nodesFileStream.is_open())
    {
        std::cerr << "Failed to open node file!" << std::endl;
        return;
    }

    while (std::getline(nodesFileStream, line))
    {
        std::istringstream iss(line);
        // Node ID parsing
        int nodeId;
        iss >> nodeId;
        // Features parsing
        std::string featuresString;
        std::getline(iss, featuresString); // jump to start of features

        // remove \t after nodeId
        if (!featuresString.empty() && featuresString[0] == '\t')
        {
            featuresString.erase(0, 1); // Remove the first character
        }

        std::vector<double> features;
        std::istringstream featuresStream(featuresString);

        // parsing feature value
        int label;
        std::string feature;
        while (std::getline(featuresStream, feature, ','))
        {
            if (feature[0] == ' ')
            {
                feature.erase(0, 1);
            }
            size_t tmp = feature.find('\t');
            if (tmp != std::string::npos)
            {
                label = std::stoi(feature.substr(tmp + 1));
                feature = feature.substr(0, tmp);
            }
            if (feature == "'#'" || feature == "#")
            {
                features.push_back(std::numeric_limits<double>::quiet_NaN()); // Replace missing feature
            }
            else
            {
                features.push_back(std::stod(feature)); // Convert to double
            }
        }

        // Add Node to the Graph
        nodes.emplace_back(nodeId, features, label);
    }
    nodesFileStream.close();
}

std::vector<int> Graph::getNodes() const
{
    std::vector<int> nodeIds;
    for (const auto &node : nodes)
    {
        nodeIds.push_back(node.getId());
    }
    return nodeIds;
}

std::vector<std::pair<int, int>> Graph::getEdges() const
{
    return edges->getEdges();
}

std::vector<int> Graph::getNeighbors(int nodeId) const
{
    return edges->getNeighbors(nodeId);
}

int Graph::getNodeCount() const
{
    return nodes.size();
}

int Graph::getEdgeCount() const
{
    auto nonConstEdges = const_cast<BasicEdges *>(this->edges.get());
    return nonConstEdges->size();
}

std::vector<double> Graph::getFeatureById(int nodeId) const
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

void Graph::updateFeatureById(int nodeId, const std::vector<double> &newFeatures)
{
    for (auto &node : nodes)
    {
        if (node.getId() == nodeId)
        {
            node.setFeatureVector(newFeatures);
            return;
        }
    }
    throw std::invalid_argument("Node ID not found");
}

void Graph::setEdgeWeight(int source, int destination, double weight)
{
    edges->setWeight(source, destination, weight);
}

double Graph::getEdgeWeight(int source, int destination) const
{
    return edges->getWeight(source, destination);
}