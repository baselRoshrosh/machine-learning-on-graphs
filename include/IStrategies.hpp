#ifndef ISTRATEGIES_HPP
#define ISTRATEGIES_HPP

#include "IGraph.hpp"

#include <memory>
#include <map>
#include <string>

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
     * @brief Constructor to initialize the strategy with a graph.
     * @param graph A shared pointer to the graph object.
     */
    IStrategies(std::shared_ptr<Graph<double>> graph) : graph(graph) {}

    /**
     * @brief Virtual destructor to support polymorphism.
     */
    virtual ~IStrategies() = default;

    /**
     * @brief Runs the strategy on the graph.
     */
    virtual void run() = 0;

    /**
     * @brief Extracts the results after running the strategy.
     * @return A modified graph with missing features filled.
     */
    virtual std::shared_ptr<Graph<double>> extractResults() const = 0;

    /**
     * @brief Configures strategy-specific parameters.
     * @param params A map of parameter names and their values.
     */
    virtual void configure(const std::map<std::string, double>& params) = 0;

    /**
     * @brief Resets the strategy to its initial state.
     */
    virtual void reset() = 0;

protected:
    std::shared_ptr<Graph<double>> graph; ///< The input graph for the strategy.
};

#endif // ISTRATEGIES_HPP
