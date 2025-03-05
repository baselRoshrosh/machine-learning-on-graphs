#ifndef ISTRATEGIES_HPP
#define ISTRATEGIES_HPP

#include "Graph.hpp"

#include <memory>
#include <map>
#include <string>
#include <cmath> //for nan

using namespace std;  


/**
 * @class IStrategies
 * @brief Interface for graph-based strategies to fill missing features.
 *
 * This interface ensures consistency among various strategies like KNN,
 * Topo2Vec, Node2Vec, and GCN. Each strategy operates on a given graph
 * and outputs a modified graph with missing features filled.
 */
class IStrategies
{
public:
    /**
     * @brief Runs the strategy on the graph.
     */
    virtual void run() = 0;

    /**
     * @brief Extracts the results after running the strategy.
     * @return A modified graph with missing features filled.
     */
    virtual shared_ptr<Graph> extractResults() const = 0;

    /**
     * @brief Configures strategy-specific parameters.
     * @param params A map of parameter names and their values.
     */
    virtual void configure(const map<string, double> &params) = 0;

    /**
     * @brief Resets the strategy to its initial state.
     */
    virtual void reset() = 0;

    void guessFeatures(int nodeId, const vector<vector<double>>& nodeList) {
        if (!graph) return;

        vector<double> nodeFeatures = graph->getFeatureById(nodeId);
        bool featuresUpdated = false;

        for (size_t i = 0; i < nodeFeatures.size(); ++i) {
            if (isnan(nodeFeatures[i])) {  
                double sum = 0.0;
                int count = 0;

                for (const auto& neighborFeatures : nodeList) {
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
            graph->updateFeatureById(nodeId, nodeFeatures);
        }
    }

protected:
    shared_ptr<Graph> graph; ///< The input graph for the strategy.
};

#endif // ISTRATEGIES_HPP
