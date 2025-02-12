#ifndef TOPO2VEC_HPP
#define TOPO2VEC_HPP

#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "IStrategies.hpp"
#include "Graph.hpp"

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
    Topo2Vec(std::shared_ptr<Graph> graph) : graph(graph) {};

    /**
     * @brief Runs the strategy on the graph.
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
    void configure(const std::map<std::string, double> &params) override;

    /**
     * @brief Resets the strategy to its initial state.
     */
    void reset() override;

    virtual ~Topo2Vec() = default;
    
protected:
    std::shared_ptr<Graph> graph; ///< the input graph for the strategy
    /*
     * tau is used to cutoff unimportant nodes from the subgraph
     * and should be in the range of 0.1-0.9 (Figure 3 in the paper).
     * lower values of tau result in slightly better quality of evaluation,
     * higher values in smaller subgraphs, meaning less memory and time needed
     */
    double tau = 0.5;              ///< configurable variable for filtering important structural nodes

    int embeddingDimensions = 128; ///< size of the embedding vector of each node. Default taken from node2vec
    int numEpochs = 5;             ///< number of gradient descent iterations. Default taken from word2vec
    int windowSize = 5;            ///< how many context nodes aroung a given node should be considered. Default taken from word2vec
    int numNegativeSamples = 5;    ///< number of randomly chosen negative samples for each positive sample. Default taken from word2vec
    double learningRate = 0.025;   ///< how fast the gradient descent should operate. Default taken from word2vec, although it gets gradually decreased there

    /**
     * creates a sample of a set of vectors
     *
     * @param setOfVectors the given total set of vectors
     * @param sampleSize the number of vectors in the sample
     * @return A set of vectors
     */
    std::vector<std::vector<double>> getSample(const std::vector<std::vector<double>> &setOfVectors, int sampleSize);

    /**
     * finds the k-most similar nodes by comparing embedding distances
     *
     * @param embeddings the embeddings <nodeID, embeddingVector> of a set of nodes
     * @param kSimilarNodes the number of similar nodes retrieved
     * @return a set of the k most similar nodes of the given embeddings
     */
    std::vector<std::vector<double>> getSimilarNodes(const std::unordered_map<int, std::vector<double>> &embeddings, int kSimilarNodes);

    /**
     * creates embeddings for the nodes following the topo2vec algorithm
     *
     * @see Topo2Vec paper DOI:https://doi.org/10.1109/TCSS.2019.2950589
     *
     * @param[in] graph the graph on which the embeddings should be created
     * @return an embedding for each of the nodes <nodeID, embeddingVector> of the graph
     */
    std::unordered_map<int, std::vector<double>> createEmbeddings(int dimensions);

    /**
     * ====== helper methods for createEmbeddings() ==========================
     */

    /**
     * Equals Algorithm 1 of the topo2vec paper.
     * It uses neighborhood affinity (NA) and subgraph affinity (SA) passing the thresshold tau
     * to create a context subgraph for each node.
     *
     * @return a vector containing the nodeIDs of context-subgraphs generated for each node in the graph
     */
    std::vector<std::vector<int>> getContextSubgraphs();

    /**
     * Equals Algorithm 2 if the topo2vec paper, also called SEARCH-procedure. Named differently for clarity
     *
     * @param[in, out] templist
     * @param[in, out] visited <nodeID, hasBeenVisited>, stores whether a given node has already been considered
     * @param[in, out] edgesInSubgraphCount how many edges are in the given subgraph as it is used to calculate the SA-score
     */
    void expandSubgraph(std::vector<int> &templist, std::unordered_map<int, bool> &visited, std::unordered_set<int> &subgraphNodes, int &edgesInSubgraphCount);

    /**
     *  Performs the skip gram algorithm with negative sampling to create an embedding for each node based on the list of context graphs using SGD.
     *
     * @see Topo2Vec paper. DOI:https://doi.org/10.1109/TCSS.2019.2950589
     * @see Node2Vec paper, on which using skip gram for topo2vec is based. DOI:https://doi.org/10.48550/arXiv.1607.00653
     * @see Word2Vec paper, on which using skip gram is based in generell. DOI:https://doi.org/10.48550/arXiv.1310.4546
     * @see word2vec commented c implementation: https://github.com/chrisjmccormick/word2vec_commented/blob/master/word2vec.c
     *
     * @param[in, out] embeddings <nodeID, embeddingVector> the embeddings to be filled
     * @param[in] subGraphs a list of context graphs, each one based on a given node in the graph
     */
    void skipGram(std::unordered_map<int, std::vector<double>> &embeddings, const std::vector<std::vector<int>> &subGraphs);

    friend class Topo2VecTest; // grant access to test class
};

#endif