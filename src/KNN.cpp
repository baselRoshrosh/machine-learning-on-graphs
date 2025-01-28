#include "../include/KNN.hpp"

/**
 * @brief Cache the neighbors of all nodes in the graph.
 *
 * @param graph The graph to process.
 */
void KNN::cacheNeighbors(const Graph& graph) {
    for (const auto& node : graph.getNodes()) {
        cachedNeighbors[node] = graph.getNeighbors(node);
    }
}

/**
 * @brief Calculate the shortest paths for all nodes up to a distance of k.
 *
 * @param graph The graph to process.
 * @param k The number of nearest neighbors to consider.
 */
void KNN::calcPaths(const Graph& graph, int k) {
    for (const auto& node : graph.getNodes()) {
        std::unordered_map<int, int> distances;
        std::queue<std::pair<int, int>> toVisit;
        //tracks how many nodes have been updated with shortest paths
        int shortestFound = 0; 

        //prepare bfs
        for (const auto& otherNode : graph.getNodes()) {
            distances[otherNode] = std::numeric_limits<int>::max();
        }

        distances[node] = 0;
        toVisit.push({node, 0});

        //Perform BFS, but stop if k nearest nodes are found
        while (!toVisit.empty() && shortestFound < k) {
            auto [current, depth] = toVisit.front();
            toVisit.pop();

            for (int neighbor : cachedNeighbors[current]) {
                if (depth + 1 < distances[neighbor]) {
                    distances[neighbor] = depth + 1;
                    toVisit.push({neighbor, depth + 1});
                    ++shortestFound; 

                    if (shortestFound >= k) {
                        break;
                    }
                }
            }
        }

        //Store the limited precomputed paths for this node
        precomputedPaths[node] = std::move(distances);
    }
}

/**
 * @brief Estimate missing features for nodes in the graph using k-nearest neighbors.
 *
 * @param graph The graph to process.
 * @param k The number of neighbors to consider.
 */
void KNN::estimateFeatures(Graph& graph, int k) {
    cacheNeighbors(graph);
    calcPaths(graph, k);

    //Initialize the set of nodes to process
    std::vector<int> nodes = graph.getNodes();
    //if a node still has a missing feature it gets revisited
    std::unordered_set<int> nodesToProcess(nodes.begin(), nodes.end());

    //avoid infinite loops, maxIterations is arbitrary and can be changed
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

