#ifndef TOPO2VEC_HPP
#define TOPO2VEC_HPP

#include "IStrategies.hpp"

class Topo2Vec : public IStrategies
{
public:
    /**
     * @brief Default constructor for Topo2Vec.
     */
    Topo2Vec() = default;

    /**
     * @brief Constructor to initialize the strategy with a graph.
     * @param graph A shared pointer to the graph object.
     */
    Topo2Vec(std::shared_ptr<Graph<double>> graph) : graph(graph) {};

    /**
     * @brief Runs the strategy on the graph.
     */
    void run() override;

    /**
     * @brief Extracts the results after running the strategy.
     * @return A modified graph with missing features filled.
     */
    std::shared_ptr<Graph<double>> extractResults() const override;

    /**
     * @brief Configures strategy-specific parameters.
     * @param params A map of parameter names and their values.
     */
    void configure(const std::map<std::string, double> &params) override;

    /**
     * @brief Resets the strategy to its initial state.
     */
    void reset() override;

private:
    std::shared_ptr<Graph<double>> graph; ///< The input graph for the strategy.
    double tau; ///< configurable variable for embedding creation

    /**
     * @brief creates a sample of a set of vectors
     * @param setOfVectors the given total set of vectors
     * @param sampleSize the number of vectors in the sample
     * @return A set of vectors
     */
    std::vector<std::vector<double>> getSample(std::vector<std::vector<double>> setOfVectors, int sampleSize);

    /**
     * @brief finds the k-most similar nodes by comparing embedding distances
     * @param embeddings the embeddings of a set of nodes
     * @param kSimilarNodes the number of similar nodes retrieved
     * @return a set of the k most similar nodes of the given embeddings
     */
    std::vector<std::vector<double>> getSimilarNodes(std::vector<std::vector<double>> embeddings, int kSimilarNodes);

    /**
     * @brief creates embeddings for the nodes following the topo2vec algorithm
     * @param graph the graph on which the embeddings should be created
     * @return an embedding for each of the nodes of the graph holding [nodeID, value1, value2, ...]
     */
    std::vector<std::vector<double>> createEmbeddings(std::shared_ptr<Graph<double>> graph);
};

#endif