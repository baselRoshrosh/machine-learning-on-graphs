#ifndef STRATEGIERUNNER_HPP
#define STRATEGIERUNNER_HPP

#include "Graph.hpp"

#include <fstream>
#include <map>
#include <cmath>
#include <iostream>

using namespace std;

/**
 * @brief A class to run a strategy on a given graph.
 *
 * This is a wrapper around a strategy that runs any strategy
 * (KNN, Topo2Vec, or Attributed Deepwalk) on a graph.
 *
 * @tparam Strategy The strategy to run on the graph.
 *
 * Example usage:
 * ```cpp
 * StrategieRunner<KNN> knn;
 * knn.configure({{"k", 5}});
 * knn.run();
 * auto graphResult = knn.extractResults()
 * knn.saveFeatures(graphResult, "knn_results.txt");
 * knn.reset();
 * ```
 *
 * @note This template avoids runtime polymorphism for performance and ensures
 * strategy operations are optimized at compile time. Therefore it adheres to the
 * IStrategies interface
 */
template <typename Strategy>
class StrategyRunner
{
private:
    Strategy strategy;

public:
    StrategyRunner(const shared_ptr<Graph> graph) : strategy(graph) {}

    void configure(const map<string, double> &params)
    {
        strategy.configure(params);
    }

    void run()
    {
        strategy.run();
    }

    void reset()
    {
        strategy.reset();
    }

    shared_ptr<Graph> extractResults() const
    {
        return strategy.extractResults();
    }

    /**
     * @brief Saves the interpreted features to a .txt file.
     *
     * This function takes the original node file and processes it to interpret the features.
     * The interpreted features replace the placeholder '#' in the original file.
     *
     * @param fileName The name of the file where the interpreted features will be saved.
     */
    void saveFeatures(const string &filename) const
    {
        ofstream outFile(filename);
        if (!outFile.is_open())
        {
            cerr << "Failed to open file for writing: " << filename << endl;
            return;
        }

        outFile << "node_id\tfeature\tlabel" << endl;

        for (const auto &nodeId : graph->getNodes())
        {
            outFile << nodeId << "\t";
            const auto &features = graph->getFeatureById(nodeId);
            for (size_t i = 0; i < features.size(); ++i)
            {
                if (isnan(features[i]))
                {
                    outFile << "#";
                }
                else
                {
                    outFile << features[i];
                }
                if (i != features.size() - 1)
                {
                    outFile << ",";
                }
            }
            outFile << "\t" << graph->getLabelById(nodeId) << endl;
        }

        outFile.close();
    }
};

#endif // STRATEGYRUNNER_HPP
