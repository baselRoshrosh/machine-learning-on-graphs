#include <queue>
#include <unordered_set>
#include <cmath>
#include <iostream>
#include <algorithm>

#include "KNN.hpp"

using namespace std;

/*
 * ======= Implementation of IStrategy Interface methods =============
 */
void KNN::run()
{
    if (!graph)
    {
        cerr << "Error: Graph is not set in KNN strategy." << endl;
        return;
    }
    
    cacheNeighbors(*graph);
    calcPaths(*graph, k);
    estimateFeatures(*graph, k);
}

shared_ptr<Graph> KNN::extractResults() const
{
    return graph;
}

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

void KNN::reset()
{
    cachedNeighbors.clear();
    precomputedPaths.clear();
    k = 15;
}

/*
 * ======= Strategy Methods ======================
 */
void KNN::cacheNeighbors(const Graph &graph)
{
    //Each node and its neighbors are stored
    for (const auto &node : graph.getNodes())
    {
        cachedNeighbors[node] = graph.getNeighbors(node);
    }
}

void KNN::calcPaths(const Graph &graph, int k)
{   
    //Calculate the shortest paths for all nodes up to a distance of k.
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
        precomputedPaths[node] = move(distances);
    }
}

// KNN-specific version of guessFeatures that takes feature vectors directly.
void guessFeaturesKNN(Graph &graph, int nodeId, const vector<vector<double>> &similarNodes) {
    vector<double> nodeFeatures = graph.getFeatureById(nodeId);
    bool featuresUpdated = false;

    for (size_t i = 0; i < nodeFeatures.size(); ++i) {
        if (isnan(nodeFeatures[i])) {
            double sum = 0.0;
            int count = 0;

            for (const auto &neighborFeatures : similarNodes) {
                if (i < neighborFeatures.size() && !isnan(neighborFeatures[i])) {
                    sum += neighborFeatures[i];
                    ++count;
                }
            }

            if (count > 0) {
                nodeFeatures[i] = sum / count;
                featuresUpdated = true;
            }
        }
    }

    if (featuresUpdated) {
        graph.updateFeatureById(nodeId, nodeFeatures);
    }
}

void KNN::estimateFeatures(Graph &graph, int k)
{
    vector<int> nodes = graph.getNodes();
    unordered_set<int> nodesToProcess(nodes.begin(), nodes.end());

    int currentIteration = 0;
    //check every node, if a node still has a missing feature it gets checked again
    //stop if a node got checked to often to avoid infinite loops
    while (!nodesToProcess.empty() && currentIteration < maxIterations) 
    {
        unordered_set<int> nextIterationNodes;
        currentIteration++;

        for (const int &node : nodesToProcess)
        {
            const auto &topoDistance = precomputedPaths[node];
            vector<pair<int, int>> neighborsSorted;

            //filter out the neighbors that are within a distance of 'k'
            for (const auto &[neighbor, distance] : topoDistance)
            {
                if (neighbor != node && distance <= k)
                {
                    neighborsSorted.emplace_back(distance, neighbor);
                }
            }

            sort(neighborsSorted.begin(), neighborsSorted.end());
            vector<vector<double>> similarNodes;

            //collect feature vectors of the closest k-neighbors
            for (size_t i = 0; i < min(k, static_cast<int>(neighborsSorted.size())); ++i)
            {
                similarNodes.push_back(graph.getFeatureById(neighborsSorted[i].second));
            }

            guessFeaturesKNN(graph, node, similarNodes);
            //revisit a node if it still has a missing feature
            for (double feature : graph.getFeatureById(node))
            {
                if (isnan(feature))
                {
                    nextIterationNodes.insert(node);
                    break;
                }
            }
        }

        nodesToProcess = move(nextIterationNodes);
    }

    if (currentIteration == maxIterations)
    {
        cerr << "Max iteration depth reached. Could not fill all features." << endl;
    }
}
