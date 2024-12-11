#include "../include/IGraph.hpp"
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
        std::vector<Edge> edges;
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
              
        std::ifstream edgesFileStream(edgesFile);
        if (!edgesFileStream.is_open()) {
            std::cerr << "Failed to open file!" << std::endl;
            return;
        }

        std::string line;
        while (std::getline(edgesFileStream, line)) {
            std::istringstream iss(line);
            int source, destination;
            if (iss >> source >> destination) {
                 //both direction edges? (because it's not directed graph)
                  edges.emplace_back(source, destination);
                  edges.emplace_back(destination, source);
            }else {
              std::cerr << "Invalid line in edge file: " << line << std::endl;
            }
        }
        edgesFileStream.close();

        std::ifstream nodesFileStream(nodesFile);
        if (!nodesFileStream.is_open()) {
            std::cerr << "Failed to open file!" << std::endl;
            return;
        }

        std::string line;
        while (std::getline(nodesFileStream, line)) {
            std::istringstream iss(line);

            // Node ID parsing
            int nodeId;
            iss >> nodeId;

            // Features parsing
            std::string featuresString;
            std::getline(iss, featuresString, '\t');  // Skips free spaces/Tabs and goes to features
            std::vector<T> features;
            std::istringstream featuresStream(featuresString);
            std::string feature;
            while (std::getline(featuresStream, feature, ',')) {
                if (feature == "#") {
                    features.push_back(static_cast<T>(-1));  // Replace missing feature
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

    std::vector<IEdge> getEdges() const override
    {
        return this->edges;
    }

    std::vector<int> getNeighbors(int nodeId) const override
    {
        // Implement neighbor retrieval logic.
        return {};
    }

    int getNodeCount() const override
    {
        return this->nodes.size();
    }

    int getEdgeCount() const override
    {
        return this->edges.size();
    }
};