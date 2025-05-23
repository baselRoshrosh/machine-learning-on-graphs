#ifndef TOPO2VEC_HPP
#define TOPO2VEC_HPP

#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "EmbeddingStrategy.hpp"
#include "Graph.hpp"

using namespace std;

class Topo2Vec : public EmbeddingStrategy
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
    Topo2Vec(shared_ptr<Graph> graph)
    {
        this->graph = graph;
    };

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

    virtual ~Topo2Vec() = default;

protected:
    /*
     * tau is used to cutoff unimportant nodes from the subgraph
     * and should be in the range of 0.1-0.9 (Figure 3 in the paper).
     * lower values of tau result in slightly better quality of evaluation,
     * higher values in smaller subgraphs, meaning less memory and time needed
     */
    double tau = 0.5; ///< configurable variable for filtering important structural nodes


    

    /**
     * creates embeddings for the nodes following the topo2vec algorithm
     *
     * @see Topo2Vec paper. DOI:https://doi.org/10.1109/TCSS.2019.2950589
     *
     * @param[in] dimensions how many dimensions an embeddings should have
     * @return an embedding for each of the nodes <nodeID, embeddingVector> of the graph
     */
    unordered_map<int, vector<double>> createEmbeddings(int dimensions);

    /**
     * ====== helper methods for createEmbeddings() ==========================
     */

    /**
     * Equals Algorithm 1 of the topo2vec paper.
     * It uses neighborhood affinity (NA) and subgraph affinity (SA) passing the thresshold tau
     * to create a context subgraph for each node.
     *
     * @see Topo2Vec paper. DOI:https://doi.org/10.1109/TCSS.2019.2950589
     *
     * @return a vector containing the nodeIDs of context-subgraphs generated for each node in the graph
     */
    vector<vector<int>> getContextSubgraphs();

    /**
     * Equals Algorithm 2 if the topo2vec paper, also called SEARCH-procedure. Named differently for clarity
     *
     * @see Topo2Vec paper. DOI:https://doi.org/10.1109/TCSS.2019.2950589
     *
     * @param[in, out] templist
     * @param[in, out] visited <nodeID, hasBeenVisited>, stores whether a given node has already been considered
     * @param[in, out] edgesInSubgraphCount how many edges are in the given subgraph as it is used to calculate the SA-score
     */
    void expandSubgraph(vector<int> &templist, unordered_map<int, bool> &visited, unordered_set<int> &subgraphNodes, int &edgesInSubgraphCount);


    friend class Topo2VecTest; // grant access to test class
};

#endif