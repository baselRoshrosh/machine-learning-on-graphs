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

        // Utility function to check if a string is an integer
        bool isInteger(const std::string &str)
        {
            if (str.empty() || ((!isdigit(str[0])) && (str[0] != '-') && (str[0] != '+')))
              return false;

            char *end = nullptr;
            std::strtol(str.c_str(), &end, 10);
            return (*end == '\0');  // Check if the entire string was parsed
        }

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
        if (!file.is_open()) {
            std::cerr << "Failed to open file!" << std::endl;
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
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

        std::ifstream nodesFileStream(nodesFile);
        if (!file.is_open()) {
            std::cerr << "Failed to open file!" << std::endl;
            return;
        }
        std::string line;
        bool nodeIdKey = false;  // Tracks if Node ID has been processed
        bool nodeLabelKey = false;  // Tracks if Label is being processed
        int currentNodeId = -1;
        std::vector<T> features;
        int label;

        while (std::getline(nodesFileStream, line)) {
                std::istringstream iss(line);
                std::string value;

            while (std::getline(iss, value, ',')) {
                    if (isInteger(value)) {
                        int intValue = std::stoi(value);

                        if (!nodeIdKey) {
                            // This is the Node ID
                            currentNodeId = intValue;
                            nodeIdKey = true;
                        }else if (nodeIdKey && !nodeLabelKey) {
                            // This is the Label
                            label = intValue;
                            nodeLabelKey = true;

                            // Finalize and reset for the next node
                            nodes.emplace_back(currentNodeId, features, label);
                            features.clear();
                            nodeIdKey = false;
                            nodeLabelKey = false;
                        }
                    }else {
                    // This is a feature
                        if (value == "#") {
                            features.push_back(static_cast<T>(-1));  // Replace missing feature
                        }else {
                            features.push_back(static_cast<T>(std::stod(value)));  // Convert to T
                        }   
                    }
            }
        }     
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