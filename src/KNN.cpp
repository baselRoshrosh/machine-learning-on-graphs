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
{    //Calculate the shortest paths for all nodes up to a distance of k.
    for (const auto &node : graph.getNodes())
    {
        unordered_map<int, int> distances;
        queue<int> toVisit;
        unordered_set<int> visited;
 
        distances[node] = 0;
        toVisit.push(node);
        visited.insert(node);
 
        int foundNeighbors = 0;
        // Perform BFS, but stop if k nearest nodes are found
        while (!toVisit.empty() && foundNeighbors < k)
        {
            int current = toVisit.front();
            toVisit.pop();
 
            for (int neighbor : cachedNeighbors[current])
            {
                if (visited.find(neighbor) == visited.end()) 
                {
                    distances[neighbor] = distances[current] + 1;
                    toVisit.push(neighbor);
                    visited.insert(neighbor);
                    foundNeighbors++;
 
                    if (foundNeighbors >= k) 
                        break; 
                }
            }
        }
        precomputedPaths[node] = move(distances);
    }
}

void KNN::estimateFeatures(Graph &graph, int k)
{
    vector<int> nodes = graph.getNodes();

    //reserve space for needsProcessing
    vector<bool> needsProcessing;
    needsProcessing.reserve(graph.getNodeCount()); 
    needsProcessing.assign(graph.getNodeCount(), true); 

    int currentIteration = 0;
    //check every node, if a node still has a missing feature it gets checked again
    //stop if a node got checked to often to avoid infinite loops
    while (currentIteration < maxIterations)
    {
        vector<bool> nextIterationProcessing;
        nextIterationProcessing.reserve(graph.getNodeCount()); 
        nextIterationProcessing.assign(graph.getNodeCount(), false); 
        //track if any node is updated to allow early stopping
        bool anyNodeProcessed = false; 

        currentIteration++;

        for (size_t i = 0; i < nodes.size(); ++i)
        {
            int node = nodes[i];

            if (!needsProcessing[i])
                continue; 

            const auto &topoDistance = precomputedPaths[node];
            priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> minHeap;

            //filter out the neighbors that are within a distance of 'k'
            for (const auto &[neighbor, distance] : topoDistance)
            {
                if (neighbor != node && distance <= k)
                {
                    minHeap.push({distance, neighbor});
                }
            }

            //collect feature vectors of the closest k-neighbors
            vector<vector<double>> similarNodes;
            for (int j = 0; j < k && !minHeap.empty(); ++j)
            {
                similarNodes.push_back(graph.getFeatureById(minHeap.top().second));
                minHeap.pop();
            }

            //revisit a node if it still has a missing feature
            bool hasMissingFeature = false;
            for (double feature : graph.getFeatureById(node))
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

                // Check if missing features remain
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

        //if no nodes were updated, exit early
        if (!anyNodeProcessed)
            break;
    }

    if (currentIteration == maxIterations)
    {
        cerr << "Max iteration depth reached. Could not fill all features." << endl;
    }
}

