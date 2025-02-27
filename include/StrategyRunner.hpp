#ifndef STRATEGIERUNNER_HPP
#define STRATEGIERUNNER_HPP

#include "Graph.hpp"

#include "interfaces/IStrategies.hpp"

/**
 * @brief Interface for graph-based strategies to fill missing features.
 *
 * This interface ensures consistency among various strategies like KNN,
 * Topo2Vec, Node2Vec, and GCN. Each strategy operates on a given graph
 * and outputs a modified graph with missing features filled.
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
 */
template <typename Strategy>
class StrategyRunner : public Strategies
{
private:
    Strategy strategy; // No heap allocation

public:
    void configure(const std::map<std::string, double> &params)
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

    std::shared_ptr<Graph> extractResults() const
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
    void saveFeatures(const std::shared_ptr<Graph> graph, const std::string &filename) const
    {
        std::ofstream outFile(filename);
        if (!outFile.is_open())
        {
            std::cerr << "Failed to open file for writing: " << filename << std::endl;
            return;
        }

        outFile << "node_id\tfeature\tlabel" << std::endl;

        for (const auto &nodeId : graph->getNodes())
        {
            outFile << nodeId << "\t";
            const auto &features = graph->getFeatureById(nodeId);
            for (size_t i = 0; i < features.size(); ++i)
            {
                outFile << features[i];
                if (i != features.size() - 1)
                {
                    outFile << ",";
                }
            }
            outFile << "\t" << graph->getLabelById(nodeId) << std::endl;
        }

        outFile.close();
    }
};

#endif // STRATEGYRUNNER_HPP
