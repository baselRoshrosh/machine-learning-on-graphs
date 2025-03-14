#include <queue>
#include <unordered_set>
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
    precomputedPaths.clear();
    k = 15;
}

/*
 * ======= Strategy Methods ======================
 */
void KNN::calcPaths(const Graph &graph, int k)
{
    // Calculate the k-nearest neighbors for each node using BFS
    for (const auto &node : graph.getNodes())
    {
        vector<int> neighbors;
        queue<int> toVisit;
        unordered_set<int> visited;

        toVisit.push(node);
        visited.insert(node);

        while (!toVisit.empty() && neighbors.size() < k)
        {
            int current = toVisit.front();
            toVisit.pop();

            for (int neighbor : graph.getNeighbors(current))
            {
                if (visited.find(neighbor) == visited.end())
                {
                    toVisit.push(neighbor);
                    visited.insert(neighbor);
                    neighbors.push_back(neighbor);
                    
                    if (neighbors.size() >= k) 
                        break;
                }
            }
        }
        precomputedPaths[node] = move(neighbors);
    }
}

void KNN::estimateFeatures(Graph &graph, int k)
{
    vector<int> nodes = graph.getNodes();
    vector<bool> needsProcessing(graph.getNodeCount(), true);
    int currentIteration = 0;

    while (currentIteration < maxIterations)
    {
        vector<bool> nextIterationProcessing(graph.getNodeCount(), false);
        bool anyNodeProcessed = false;
        currentIteration++;

        for (size_t i = 0; i < nodes.size(); ++i)
        {
            int node = nodes[i];
            if (!needsProcessing[i]) continue;

            const auto &neighbors = precomputedPaths[node];

            vector<vector<double>> similarNodes;
            for (int j = 0; j < min(k, (int)neighbors.size()); ++j)
            {
                similarNodes.push_back(graph.getFeatureById(neighbors[j]));
            }

            bool hasMissingFeature = false;
            auto features = graph.getFeatureById(node);
            for (double feature : features)
            {
                if (isnan(feature))
                {
                    hasMissingFeature = true;
                    break;
                }
            }

            if (hasMissingFeature)
            {
                guessFeatures(node, similarNodes);
                anyNodeProcessed = true;

                // Check again if there are still missing features
                for (double feature : graph.getFeatureById(node))
                {
                    if (isnan(feature))
                    {
                        nextIterationProcessing[i] = true;
                        break;
                    }
                }
            }
        }

        needsProcessing = move(nextIterationProcessing);
        if (!anyNodeProcessed) break;
    }

    if (currentIteration == maxIterations)
    {
        cerr << "Max iteration depth reached. Could not fill all features." << endl;
    }
}