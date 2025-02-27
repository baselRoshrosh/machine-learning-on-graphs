#ifndef STRATEGIES_HPP
#define STRATEGIES_HPP

#include "Graph.hpp"

/**
 * @brief Interface for graph-based strategies to fill missing features.
 *
 * This interface ensures consistency among various strategies like KNN,
 * Topo2Vec, Node2Vec, and GCN. Each strategy operates on a given graph
 * and outputs a modified graph with missing features filled.
 *
 * Example usage:
 * ```cpp
 * Strategies<KNNStrategy> knn;
 * knn.configure({{"k", 5}});
 * knn.run();
 * knn.saveFeatures("knn.txt");
 *
 * Strategies<Topo2VecStrategy> topo;
 * topo.run();
 * topo.saveFeatures("topo.txt");
 * ```
 */
template <typename Strategy>
class Strategies
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

    void saveFeatures(const std::string &filename) const
    {
        strategy.saveFeatures(filename);
    }
};

#endif // STRATEGIES_HPP
