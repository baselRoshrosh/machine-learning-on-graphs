#ifndef KNN_HPP
#define KNN_HPP

#include <iostream>
#include <cmath>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <queue>

#include "IStrategies.hpp"
#include "Graph.hpp"


/**
 * @class KNN
 * @brief Implementation of the K-Nearest Neighbors algorithm for feature estimation.
 */
class KNN : public IStrategies {
public:
    /**
     * @brief Default constructor.
     */
    KNN(std::shared_ptr<Graph> g) { graph = g; }

    /**
     * @brief Runs the KNN strategy.
     */
    void run() override;

    /**
     * @brief Extracts the results after running the strategy.
     * @return A modified graph with missing features filled.
     */
    std::shared_ptr<Graph> extractResults() const override;

    /**
     * @brief Configures strategy-specific parameters.
     * @param params A map of parameter names and their values.
     */
    void configure(const std::map<std::string, double>& params) override;

    /**
     * @brief Resets the strategy to its initial state.
     */
    void reset() override;

private:
    int k = 15; 
    //Cache for neighbors and path to avoid repeatedly calculating them
    std::unordered_map<int, std::vector<int>> cachedNeighbors;          
    std::unordered_map<int, std::unordered_map<int, int>> precomputedPaths;

    /**
     * @brief Cache the neighbors of all nodes in the graph.
     *
     * @param graph The graph to process.
     */
    void cacheNeighbors(const Graph& graph);

    /**
     * @brief Calculate the shortest paths for all nodes up to a distance of k.
     *
     * @param graph The graph to process.
     * @param k The number of nearest neighbors to consider.
     */
    void calcPaths(const Graph& graph, int k);

    /**
     * @brief Estimate missing features for nodes in the graph using k-nearest neighbors.
     *
     * @param graph The graph to process.
     * @param k The number of neighbors to consider.
     */
    void estimateFeatures(Graph& graph, int k);
};


#endif // KNN_HPP

