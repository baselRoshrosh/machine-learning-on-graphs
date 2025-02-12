#ifndef GRAPH_TPP
#define GRAPH_TPP

#include "Graph.hpp"

#include <limits>
#include <memory>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cctype>
#include <cstdlib>

/**
 * @class Graph
 * @brief Concrete implementation of the IGraph interface.
 */
template <typename T>
class Graph : public IGraph<T>
{
private:
        std::unique_ptr<IEdges> edges;
        std::vector<Node<T>> nodes;

public:
    /**
     * @brief Constructs a graph by parsing from txt files.
     *
     * @param nodesFile The file containing node information.
     * @param edgesFile The file containing edge information.
     */
    Graph(const std::string &nodesFile, const std::string &edgesFile)
    {
        // read edge file
        std::ifstream edgesFileStream(edgesFile);
        edges = std::make_unique<BasicEdges>();
        if (!edgesFileStream.is_open()) {
            std::cerr << "Failed to open file!" << std::endl;
            return;
        }

        std::string line;
        while (std::getline(edgesFileStream, line)) {
            std::istringstream iss(line);
            int source, destination;
            if (iss >> source >> destination) {
            // Add edge only if it doesn't already exist
                if (!edges->isEdge(source, destination) && !edges->isEdge(destination, source)) {
                    edges->addEdge(source, destination);
                }
            }else {
              std::cerr << "Invalid line in edge file: " << line << std::endl;
            }
        }
        edgesFileStream.close();

        // read node file
        std::ifstream nodesFileStream(nodesFile);
        if (!nodesFileStream.is_open()) {
            std::cerr << "Failed to open file!" << std::endl;
            return;
        }

        
        while (std::getline(nodesFileStream, line)) {
            std::istringstream iss(line);

            // Node ID parsing
            int nodeId;
            iss >> nodeId;

            // Features parsing
            std::string featuresString;
            std::getline(iss, featuresString, '\t');  //jump to start of features
            std::vector<T> features;
            std::istringstream featuresStream(featuresString);

            //parsing feature value
            std::string feature;
            while (std::getline(featuresStream, feature, ',')) {
                if (feature == "#") {
                    features.push_back(static_cast<T>(std::numeric_limits<T>::quiet_NaN()));  // Replace missing feature
                } else {
                    features.push_back(static_cast<T>(std::stod(feature)));  // Convert to T
                }
            }

            // Label Parsing
            int label;
            iss >> label;

            // Add Node to the Graph
            nodes.emplace_back(nodeId, features, label);
        }
        nodesFileStream.close();
    }


    std::vector<int> getNodes() const override
    {
        std::vector<int> nodeIds;
        for (const auto &node : this->nodes)
        {
            nodeIds.push_back(node.getId());
        }
        return nodeIds;
    }

    std::vector<std::pair<int, int>> getEdges() const override
    {
        return edges->getEdges();
    }

    std::vector<int> getNeighbors(int nodeId) const override
    {
        return edges->getNeighbors(nodeId);
    }

    int getNodeCount() const override
    {
        return this->nodes.size();
    }

    int getEdgeCount() const override
    {
        return edges->getEdges().size();
    }


};
#endif // GRAPH_TPP
