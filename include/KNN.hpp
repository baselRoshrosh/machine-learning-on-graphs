#ifndef KNN_HPP
#define KNN_HPP

#include <vector>
#include <memory>
#include <unordered_map>

#include "interfaces/IStrategies.hpp"
#include "Graph.hpp"

using namespace std;

/**
 * @class KNN
 * @brief Implementation of the K-Nearest Neighbors algorithm for feature estimation.
 */
class KNN : public IStrategies
{
private:
    int k = 15;
    int maxIterations = 10; // avoid infinite loops, the nax iterations is arbitrary and can be changed
    unordered_map<int, vector<int>> precomputedPaths;

    /**
     * @brief Cache the neighbors of all nodes in the graph.
     *
     * @param graph The graph to process.
     */
    void cacheNeighbors(const Graph &graph);

    /**
     * @brief Calculate the shortest paths for all nodes up to a distance of k.
     *
     * @param graph The graph to process.
     * @param k The number of nearest neighbors to consider.
     */
    void calcPaths(const Graph &graph, int k);

    /**
     * @brief Estimate missing features for nodes in the graph using k-nearest neighbors.
     *
     * @param graph The graph to process.
     * @param k The number of neighbors to consider.
     */
    void estimateFeatures(Graph &graph, int k);

public:
    /**
     * @brief Default constructor.
     */
    KNN(shared_ptr<Graph> g) { graph = g; }

    /**
     * @brief Runs the KNN strategy.
     */
    void run() override;

    /**
     * @brief Extracts the results after running the strategy.
     * @return A modified graph with missing features filled.
     */
    shared_ptr<Graph> extractResults() const override;

    /**
     * @brief Configures strategy-specific parameters.
     * @param params A map of parameter names and their values.
     */
    void configure(const map<string, double> &params) override;

    /**
     * @brief Resets the strategy to its initial state.
     */
    void reset() override;
};

#endif // KNN_HPP