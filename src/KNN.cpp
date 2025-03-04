#include "KNN.hpp"

using namespace std;

/**
 * @brief Runs the KNN strategy.
 */
void KNN::run()
{
    if (!graph)
    {
        cerr << "Error: Graph is not set in KNN strategy." << endl;
        return;
    }
    estimateFeatures(*graph, k);
}

/**
 * @brief Extracts the results after running the strategy.
 * @return A modified graph with missing features filled.
 */
shared_ptr<Graph> KNN::extractResults() const
{
    return graph;
}

/**
 * @brief Configures strategy-specific parameters.
 * @param params A map of parameter names and their values.
 */
void KNN::configure(const map<string, double> &params)
{
    if (params.find("k") != params.end())
    {
        k = static_cast<int>(params.at("k"));
    }
    if (params.find("maxIterations") != params.end())
    {
        int newMaxIterations = static_cast<int>(params.at("maxIterations"));
        if (newMaxIterations > 0)
        {
            maxIterations = newMaxIterations;
        }
        else
        {
            cerr << "Warning: maxIterations must be positive. Keeping the previous value: "
                      << maxIterations << endl;
        }
    }
}

/**
 * @brief Resets the strategy to its initial state.
 */
void KNN::reset()
{
    cachedNeighbors.clear();
    precomputedPaths.clear();
}

/**
 * @brief Cache the neighbors of all nodes in the graph.
 *
 * @param graph The graph to process.
 */
void KNN::cacheNeighbors(const Graph &graph)
{
    for (const auto &node : graph.getNodes())
    {
        cachedNeighbors[node] = graph.getNeighbors(node);
    }
}

/**
 * @brief Calculate the shortest paths for all nodes up to a distance of k.
 *
 * @param graph The graph to process.
 * @param k The number of nearest neighbors to consider.
 */
void KNN::calcPaths(const Graph &graph, int k)
{
    for (const auto &node : graph.getNodes())
    {
        unordered_map<int, int> distances;
        queue<pair<int, int>> toVisit;
        // tracks how many nodes have been updated with shortest paths
        int shortestFound = 0;

        // prepare bfs
        for (const auto &otherNode : graph.getNodes())
        {
            distances[otherNode] = numeric_limits<int>::max();
        }

        distances[node] = 0;
        toVisit.push({node, 0});

        // Perform BFS, but stop if k nearest nodes are found
        while (!toVisit.empty() && shortestFound < k)
        {
            auto [current, depth] = toVisit.front();
            toVisit.pop();

            for (int neighbor : cachedNeighbors[current])
            {
                if (depth + 1 < distances[neighbor])
                {
                    distances[neighbor] = depth + 1;
                    toVisit.push({neighbor, depth + 1});
                    ++shortestFound;

                    if (shortestFound >= k)
                    {
                        break;
                    }
                }
            }
        }

        // Store the limited precomputed paths for this node
        precomputedPaths[node] = move(distances);
    }
}

/**
 * @brief Estimate missing features for nodes in the graph using k-nearest neighbors.
 *
 * @param graph The graph to process.
 * @param k The number of neighbors to consider.
 */
void KNN::estimateFeatures(Graph &graph, int k)
{
    cacheNeighbors(graph);
    calcPaths(graph, k);

    // Initialize the set of nodes to process
    vector<int> nodes = graph.getNodes();
    // if a node still has a missing feature it gets revisited
    unordered_set<int> nodesToProcess(nodes.begin(), nodes.end());

    int currentIteration = 0;

    while (!nodesToProcess.empty() && currentIteration < maxIterations)
    {
        unordered_set<int> nextIterationNodes;
        currentIteration++;

        for (const int &node : nodesToProcess)
        {
            const auto &topoDistance = precomputedPaths[node];

            // filter and sort neighbors based on their distances

            vector<pair<int, int>> neighborsSorted;
            for (const auto &[neighbor, distance] : topoDistance)
            {
                if (neighbor != node && distance <= k)
                {
                    neighborsSorted.emplace_back(distance, neighbor);
                }
            }

            sort(neighborsSorted.begin(), neighborsSorted.end());

            vector<int> knn;
            for (size_t i = 0; i < min(k, static_cast<int>(neighborsSorted.size())); ++i)
            {
                knn.push_back(neighborsSorted[i].second);
            }

            vector<double> nodeFeatures = graph.getFeatureById(node);
            bool featureIsMissing = false;
            bool featuresUpdated = false;

            // estimate missing features and skip the feature if it is not missing
            for (size_t i = 0; i < nodeFeatures.size(); ++i)
            {
                if (isnan(nodeFeatures[i]))
                {
                    double sum = 0.0;
                    int count = 0;

                    // gather all neighbors and replace the missing features with the average of the neighbors
                    for (int neighborId : knn)
                    {
                        const auto &neighborFeatures = graph.getFeatureById(neighborId);
                        if (!isnan(neighborFeatures[i]))
                        {
                            sum += neighborFeatures[i];
                            ++count;
                        }
                    }

                    // only update features if valid neighbors exist that have that feature
                    if (count > 0)
                    {
                        nodeFeatures[i] = sum / count;
                        featuresUpdated = true;
                    }
                    else
                    {
                        featureIsMissing = true;
                    }
                }
            }

            if (featuresUpdated)
            {
                graph.updateFeatureById(node, nodeFeatures);
            }

            // queue node to get revisited
            if (featureIsMissing)
            {
                nextIterationNodes.insert(node);
            }
        }

        nodesToProcess = move(nextIterationNodes);
    }

    if (currentIteration == maxIterations)
    {
        cerr << "Max iteration depth reached. Could not fill all features." << endl;
    }
}
