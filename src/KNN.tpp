#ifndef KNN_TPP
#define KNN_TPP

#include "../include/KNN.hpp"

/**
 * @class KNN
 * @brief Concrete implementation of the KNN interface.
 */
void KNN::cacheNeighbors(const Graph& graph) {
    for (const auto& node : graph.getNodes()) {
        cachedNeighbors[node] = graph.getNeighbors(node);
    }
}

//BFS and precompute all the path 
void KNN::calcPaths(const Graph& graph) {
    for (const auto& node : graph.getNodes()) {
        std::unordered_map<int, int> distances;
        std::queue<std::pair<int, int>> toVisit;

        for (const auto& otherNode : graph.getNodes()) {
            distances[otherNode] = std::numeric_limits<int>::max();
        }

        distances[node] = 0;
        toVisit.push({node, 0});

        while (!toVisit.empty()) {
            auto [current, depth] = toVisit.front();
            toVisit.pop();

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

//Estimate features for nodes in a graph using k-nearest neighbors
void KNN::estimateFeatures(Graph& graph, int k) {
    cacheNeighbors(graph);
    calcPaths(graph);

    //Initialize the set of nodes to process
    std::vector<int> nodes = graph.getNodes();
    //if a node still has a missing feature it gets revisted
    std::unordered_set<int> nodesToProcess(nodes.begin(), nodes.end());

    //avoid infin loobs
    int maxIterations = 10;
    int currentIteration = 0;

    while (!nodesToProcess.empty() && currentIteration < maxIterations) {
        std::unordered_set<int> nextIterationNodes;
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

            std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<>> nearestNeighbors;

            for (const auto& [neighbor, distance] : topoDistance) {
                if (neighbor != node && distance <= k) {
                    nearestNeighbors.push({distance, neighbor});
                    if (nearestNeighbors.size() > k) {
                        nearestNeighbors.pop();  // Remove the farthest neighbor if there are more than k
                    }
                }
            }

            // Collect the k-nearest neighbors from the priority queue
            std::vector<int> knn;
            while (!nearestNeighbors.empty()) {
                knn.push_back(nearestNeighbors.top().second);
                nearestNeighbors.pop();
            }

            std::vector<double> nodeFeatures = graph.getFeatureById(node);
            bool featureIsMissing = false;
            bool featuresUpdated = false;

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

                    //only update features if valid neighbors exist that have that feature
                    if (count > 0) {
                        nodeFeatures[i] = sum / count;
                        featuresUpdated = true;
                    } else {
                        featureIsMissing = true;
                    }
                }
            }

            if (featuresUpdated) {
                graph.updateFeatureById(node, nodeFeatures);
            }
                        
            //queue node to get revisited
            if (featureIsMissing) {
                nextIterationNodes.insert(node);
            }
        }

        nodesToProcess = std::move(nextIterationNodes);
    }

    if (currentIteration == maxIterations) {
        std::cerr << "Max iteration depth reached. Could not fill all features." << std::endl;
    }
}

#endif // KNN_TPP
