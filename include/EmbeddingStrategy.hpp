#ifndef EMBEDDING_STRATEGY_HPP
#define EMBEDDING_STRATEGY_HPP

#include "interfaces/IStrategies.hpp"
#include "Graph.hpp"
#include <vector>
#include <unordered_map>
#include <queue>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <random>

using namespace std;

class EmbeddingStrategy : public IStrategies
{
public:
    // Inherit the pure virtual methods from IStrategies (run, extractResults, configure, reset)
    // They can remain abstract here if not common to all or can be partially implemented.
    virtual ~EmbeddingStrategy() = default;

protected:
    // Common parameters for embedding-based strategies:

    int embeddingDimensions = 128; ///< size of the embedding vector of each node. Default taken from node2vec
    int numEpochs = 5;             ///< number of gradient descent iterations. Default taken from word2vec
    int sampleSize = 256;          ///< number of samples to be taken for each node.
    int k = 5;                     ///< number of similar nodes to be retrieved.
    int windowSize = 5;            ///< how many context nodes aroung a given node should be considered. Default taken from word2vec
    int numNegativeSamples = 5;    ///< number of randomly chosen negative samples for each positive sample. Default taken from word2vec
    double learningRate = 0.025;   ///< how fast the gradient descent should operate. Default taken from word2vec, although it gets gradually decreased there

    /**
     *  Performs the skip gram algorithm with negative sampling to create an embedding for each node based on the list of context graphs using SGD.
     *
     * @see Topo2Vec paper. DOI:https://doi.org/10.1109/TCSS.2019.2950589
     * @see Node2Vec paper, on which using skip gram for topo2vec is based. DOI:https://doi.org/10.48550/arXiv.1607.00653
     * @see Word2Vec paper, on which using skip gram is based in generell. DOI:https://doi.org/10.48550/arXiv.1310.4546
     * @see Word2Vec explained. DOI:https://doi.org/10.48550/arXiv.1402.3722
     * @see Word2Vec original code: https://github.com/tmikolov/word2vec/blob/master/word2vec.c
     * @see Word2Vec commented c implementation: https://github.com/chrisjmccormick/word2vec_commented/blob/master/word2vec.c
     *
     * @param[in, out] embeddings <nodeID, embeddingVector> the embeddings to be filled
     * @param[in] subGraphs a list of context graphs, each one based on a given node in the graph
     */
    void skipGram(unordered_map<int, vector<double>> &embeddings,
                  const vector<vector<int>> &subGraphs)
    {
        for (int epoch = 0; epoch < numEpochs; ++epoch)
        {
            for (const auto &subGraph : subGraphs)
            {
                for (size_t i = 0; i < subGraph.size(); ++i)
                {
                    int targetNode = subGraph[i];
                    // For each node in the window around the target:
                    for (int j = -windowSize; j <= windowSize; ++j)
                    {
                        if (j == 0 || (int)i + j < 0 || (int)i + j >= (int)subGraph.size())
                            continue;
                        int contextNode = subGraph[i + j];
                        // Positive example: update embeddings with label = 1
                        updateEmbeddings(embeddings, embeddingDimensions, targetNode, contextNode, 1, learningRate);
                        // Negative sampling:
                        vector<int> negativeSamples = getNegativeSamples(graph, targetNode, numNegativeSamples);
                        for (int negativeNode : negativeSamples)
                        {
                            updateEmbeddings(embeddings, embeddingDimensions, targetNode, negativeNode, 0, learningRate);
                        }
                    }
                }
            }
        }
    }

    /**
     * creates a randomized embedding of the given dimension for each node
     *
     * @param graph[in] the original graph
     * @param embeddings[in, out] pointer to the embeddings<nodeID, embedding>
     * @param dimensions[in] how many dimensions an embedding should have
     */
    static unordered_map<int, vector<double>> initializeEmbeddings(
        shared_ptr<Graph> graph, int dimensions) 
    {
        unordered_map<int, vector<double>> embeddings;
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<double> dist(-0.5 / dimensions, 0.5 / dimensions);

        for (int node : graph->getNodes())
        {
            vector<double> vec(dimensions);
            for (double &val : vec)
            {
                val = dist(gen);
            }
            embeddings[node] = vec;
        }

        return embeddings;
    }

    /**
     * creates a sample of a set of vectors
     *
     * @param embeddings the given total set of vectors (embeddings)
     * @param sampleSize the number of vectors in the sample
     * @return A set of vectors
     */
    unordered_map<int, vector<double>> getSample(const unordered_map<int, vector<double>> &embeddings, int sampleSize)
    {
        unordered_map<int, vector<double>> sample;

        // filter edge cases
        if (embeddings.empty() || sampleSize <= 0)
            return sample;

        vector<int> keys;
        for (const auto &pair : embeddings)
        {
            keys.push_back(pair.first);
        }

        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(0, keys.size() - 1);

        while (sample.size() < sampleSize && sample.size() < embeddings.size())
        {
            int randomIndex = dis(gen);
            sample.insert({keys[randomIndex], embeddings.at(keys[randomIndex])});
        }

        return sample;
    }

    /**
     * Finds the k-most similar nodes to a given query vector by comparing cosine similarities.
     *
     * @param embeddings the embeddings <nodeID, embeddingVector> of a set of nodes.
     * @param queryVector the feature vector (or embedding) against which the similarities are computed.
     * @param kSimilarNodes the number of similar nodes to retrieve.
     * @return a vector of feature vectors corresponding to the actual features of the top-k most similar nodes,
     *         as obtained from the graph.
     */
    vector<vector<double>> getFeaturesOfSimilarNodes(
        const unordered_map<int, vector<double>> &embeddings,
        const vector<double> &queryVector,
        int kSimilarNodes)
    {
        if (embeddings.empty() || queryVector.empty() || kSimilarNodes <= 0)
            return {};
        double sumQuery = accumulate(queryVector.begin(), queryVector.end(), 0.0,
                                          [](double acc, double v)
                                          { return acc + v * v; });
        double normQuery = sqrt(sumQuery);
        if (normQuery == 0)
            return {};
        using SimilarityPair = pair<double, int>;
        priority_queue<SimilarityPair, vector<SimilarityPair>, greater<>> minHeap;
        for (const auto &[nodeID, embedding] : embeddings)
        {
            if (embedding.size() != queryVector.size() || embedding == queryVector)
                continue;
            double dot = 0.0, sumCandidate = 0.0;
            for (size_t i = 0; i < embedding.size(); ++i)
            {
                dot += queryVector[i] * embedding[i];
                sumCandidate += embedding[i] * embedding[i];
            }
            double normCandidate = sqrt(sumCandidate);
            if (normCandidate == 0)
                continue;
            double cosineSimilarity = dot / (normQuery * normCandidate);
            if (minHeap.size() < static_cast<size_t>(kSimilarNodes))
            {
                minHeap.emplace(cosineSimilarity, nodeID);
            }
            else if (cosineSimilarity > minHeap.top().first)
            {
                minHeap.pop();
                minHeap.emplace(cosineSimilarity, nodeID);
            }
        }
        vector<vector<double>> topKSimilar;
        while (!minHeap.empty())
        {
            int nodeID = minHeap.top().second;
            minHeap.pop();
            topKSimilar.push_back(graph->getFeatureById(nodeID)); // returns the node's actual features
        }
        reverse(topKSimilar.begin(), topKSimilar.end());
        return topKSimilar;
    }

    /**
     * updates Embeddings based on the connection of two nodes
     *
     * @param embeddings[in, out] pointer to the embeddings<nodeID, embedding>
     * @param dimensions[in] how many dimensions an embedding has
     * @param targetNode[in]
     * @param contextNode[in]
     * @param label[in] whether it is a positive (1) or negative (0) example
     * @param learningRate[in] how large the gradient descent steps should be
     */
    void updateEmbeddings(unordered_map<int, vector<double>> &embeddings,
                          int dimensions, int targetNode, int contextNode,
                          double label, double lr)
    {
        vector<double> &targetVec = embeddings[targetNode];
        vector<double> &contextVec = embeddings[contextNode];
        double dot = inner_product(targetVec.begin(), targetVec.end(), contextVec.begin(), 0.0);
        double score = 1.0 / (1.0 + exp(-dot)); // sigmoid function
        double gradient = (label - score) * lr;
        for (int i = 0; i < dimensions; ++i)
        {
            double temp = targetVec[i];
            targetVec[i] += gradient * contextVec[i];
            contextVec[i] += gradient * temp;
        }
    }

    /**
     * gets a number of negative samples for a specified node
     *
     * @param graph the full graph
     * @param excludeNode the node to get negative samples for
     * @param numSamples how many negative samples should be created
     *
     * @return
     */
    vector<int> getNegativeSamples(shared_ptr<Graph> graph, int excludeNode, int numSamples)
    {
        std::vector<int> negativeSamples;

        if (!graph) {
            return negativeSamples;  // Return empty vector to avoid segfault
        }

        std::vector<int> nodes = graph->getNodes();
        if (nodes.empty()) {
            return negativeSamples;
        }

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(0, nodes.size() - 1);

        while (negativeSamples.size() < static_cast<size_t>(numSamples))
        {
            int sampledNode = nodes[dist(gen)];
            if (sampledNode != excludeNode)
                negativeSamples.push_back(sampledNode);
        }

        return negativeSamples;
    }

};

#endif // EMBEDDING_STRATEGY_HPP
