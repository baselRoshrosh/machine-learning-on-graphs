#ifndef ATTRIBUTED_DEEPWALK_HPP
#define ATTRIBUTED_DEEPWALK_HPP

#include "interfaces/IStrategies.hpp"
#include "EmbeddingStrategy.hpp"

#include <vector>
#include <unordered_map>

using namespace std;

class AttributedDeepwalk : public EmbeddingStrategy
{
public:
    /**
     * @brief Constructor to initialize the strategy with a graph.
     * @param graph A shared pointer to the graph object.
     */
    AttributedDeepwalk(shared_ptr<Graph> graph) : graph(graph) {};

    /**
     * @brief Runs the strategy on the graph.
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

    /**
     * Calculates an Alias Table for each node given the edge weights of neigbors.
     *
     * @see Probability calculation in ADW paper. DOI:https://doi.org/10.1007/s00607-021-00982-2
     * @see Alias Method: https://en.wikipedia.org/wiki/Alias_method
     * @see Implementation of Alias Table in c++: https://gist.github.com/Liam0205/0b5786e9bfc73e75eb8180b5400cd1f8
     */
    void computeAliasTables();

protected:
    shared_ptr<Graph> graph; ///< The input graph for the strategy.
    unordered_map<int, vector<pair<double, size_t>>> aliasTables;

    double fusionCoefficient = 0.5; ///< tradeoff between structure and feature similarity when calculating weights. Default taken from ADW paper
    double coverDepth = 2;          ///< size of cover of a node when calculating structural similarity. Default taken from ADW paper

    int walkLength = 80;   ///< how long a random walk should be. Default taken from ADW paper
    int walksPerNode = 10; ///< how many random walks per node should be performed. Default taken from ADW paper

    /**
     * Performs the "Combination of Structural and Attributed DeepWalk" Algorithm.
     * Creates an embedding for each node.
     *
     * @see Attributed Deepwalk Paper. DOI:https://doi.org/10.1007/s00607-021-00982-2
     *
     * @return the embeddings <nodeID, nodeEmbedding> of the nodes
     */
    unordered_map<int, vector<double>> csadw();


    /**
     * Calculates the ADW weight matrix for a given graph.
     * Edge weights between nodes correspond to the strucural and feature similarity between the nodes
     *
     * @see The methods for calculating edge weights are listed in the ADW paper. DOI:https://doi.org/10.1007/s00607-021-00982-2
     */
    void calculateWeightMatrix();

    /**
     * Calculates an Alias Table for each node given the edge weights of neigbors.
     *
     * @see Probability calculation in ADW paper. DOI:https://doi.org/10.1007/s00607-021-00982-2
     * @see Alias Method: https://en.wikipedia.org/wiki/Alias_method
     * @see Implementation of Alias Table in c++: https://gist.github.com/Liam0205/0b5786e9bfc73e75eb8180b5400cd1f8
     *
     * @return a map of nodeIDs to their respective alias table
     */
    unordered_map<int, vector<pair<double, size_t>>> getAliasTables() const;

    /**
     * Performs a random walk starting at a given Node
     *
     * @param startNodeID the node from where to start the random walk
     * @return a list of nodeIDs that were passed on the random walk
     */
    vector<int> randomWalk(int startNodeID);

    /*
     *  ========= helper functions ==========
     */

    /**
     * Calculates Attribute Similarity. The paper uses Jaccard-coefficient which can't be used for numerical values.
     * Thus we are using Cosine-Similarity and potentially try Pearson-Coefficient-Similarity
     *
     * @param node1 first node to consider for similarity
     * @param node2 second node to consider for similarity
     * @return the cosine similarity of the features of the nodes
     */
    double measuring_attribute_similarity(int node1, int node2) const;

    /**
     * Calculates Structural Similarity by calculating the overlap coefficient of the respective nodes
     *
     * @param node1 first node to consider for similarity
     * @param node2 second node to consider for similarity
     * @return the overlap coefficient of the two nodes
     */
    double measuring_structural_similarity(int node1, int node2) const;
};

#endif