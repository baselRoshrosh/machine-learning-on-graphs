#include <iostream>
#include <cmath>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <algorithm>
#include <limits>
#include <iomanip>
#include <fstream>

#include "../src/Graph.tpp"
#include "../src/BasicEdges.cpp"

#include <chrono>

//Cache for neighbors and path to avoid repeatedly calculating them
std::unordered_map<int, std::vector<int>> cachedNeighbors;
std::unordered_map<int, std::unordered_map<int, int>> precomputedPaths;

void cacheNeighbors(const Graph<double>& graph) {
    for (const auto& node : graph.getNodes()) {
        cachedNeighbors[node] = graph.getNeighbors(node);
    }
}

//BFS and precompute all the path 
void calcPaths(const Graph<double>& graph) {
    for (const auto& node : graph.getNodes()) {
        std::unordered_map<int, int> distances;
        std::queue<std::pair<int, int>> toVisit;
        //prep BFS
        for (const auto& otherNode : graph.getNodes()) {
            distances[otherNode] = std::numeric_limits<int>::max();
        }

        distances[node] = 0;
        toVisit.push({node, 0});
        //BFS
        while (!toVisit.empty()) {
            auto [current, depth] = toVisit.front();
            toVisit.pop();
            //only do a k deep BFS

            for (int neighbor : cachedNeighbors[current]) {
                if (distances[neighbor] == std::numeric_limits<int>::max()) {
                    distances[neighbor] = depth + 1;
                    toVisit.push({neighbor, depth + 1});
                }
            }
        }

        precomputedPaths[node] = std::move(distances);
    }
}

template <typename T>
void estimateFeatures(Graph<T>& graph, int k) {
    cacheNeighbors(graph);
    calcPaths(graph);

    //Initialize the set of nodes to process
    std::vector<int> nodes = graph.getNodes();
    //if a node still has a missing feature it gets revisted
    std::unordered_set<int> nodesToProcess(nodes.begin(), nodes.end());

    int maxIterations = 20; //Avoid infinite loops
    int currentIteration = 0;

    while (!nodesToProcess.empty() && currentIteration < maxIterations) {
        std::unordered_set<int> nextIterationNodes; //Nodes that still need processing
        currentIteration++;

        for (const int& node : nodesToProcess) {
            const auto& topoDistance = precomputedPaths[node];

            //filter and sort neighbors based on their distances
            std::vector<std::pair<int, int>> distances;
            for (const auto& [neighbor, distance] : topoDistance) {
                if (neighbor != node && distance <= k) {
                    distances.emplace_back(distance, neighbor);
                }
            }

            std::sort(distances.begin(), distances.end());

            //only choose the k nearest neighbors
            std::vector<int> knn;
            for (size_t i = 0; i < std::min<size_t>(k, distances.size()); ++i) {
                knn.push_back(distances[i].second);
            }

            std::vector<T> nodeFeatures = graph.getFeatureById(node);
            bool featureIsMissing = false;

            //estimate missing features and skip the feature if it is not missing
            for (size_t i = 0; i < nodeFeatures.size(); ++i) {
                if (std::isnan(nodeFeatures[i])) {

                    double sum = 0.0;
                    int count = 0;

                    //gather all neighbors and replace the missing features with the average of the neighbors
                    for (int neighborId : knn) {
                        const auto& neighborFeatures = graph.getFeatureById(neighborId);
                        if (!std::isnan(neighborFeatures[i])) {
                            sum += neighborFeatures[i];
                            ++count;
                        }
                    }

                    // Only update features if valid neighbors exist that have that feature
                    if (count > 0) {
                        nodeFeatures[i] = sum / count;
                    } else {
                        featureIsMissing = true; 
                    }
                }
            }

            graph.updateFeatureById(node, nodeFeatures);

            //queue node to get revisited
            if (featureIsMissing) {
                nextIterationNodes.insert(node);
            }
        }

        nodesToProcess = std::move(nextIterationNodes); 
    }

    if (currentIteration == maxIterations) {
        std::cerr << "max iteration depth, could not fill all features" << std::endl;
    }
}
