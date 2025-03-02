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

class EmbeddingStrategy : public IStrategies {
public:
    // Inherit the pure virtual methods from IStrategies (run, extractResults, configure, reset)
    // They can remain abstract here if not common to all or can be partially implemented.
    virtual ~EmbeddingStrategy() = default;

protected:
    // Common parameters for embedding-based strategies:
    int embeddingDimensions = 128; ///< dimension size for each embedding vector
    int numEpochs = 5;             ///< number of gradient descent iterations
    int windowSize = 5;            ///< context window size
    int numNegativeSamples = 5;    ///< number of negative samples for skip-gram
    double learningRate = 0.025;   ///< learning rate for skip-gram

    

    // SkipGram implementation:
    void skipGram(std::unordered_map<int, std::vector<double>> &embeddings,
                  const std::vector<std::vector<int>> &subGraphs) {
        for (int epoch = 0; epoch < numEpochs; ++epoch) {
            for (const auto &subGraph : subGraphs) {
                for (size_t i = 0; i < subGraph.size(); ++i) {
                    int targetNode = subGraph[i];
                    // For each node in the window around the target:
                    for (int j = -windowSize; j <= windowSize; ++j) {
                        if (j == 0 || (int)i + j < 0 || (int)i + j >= (int)subGraph.size())
                            continue;
                        int contextNode = subGraph[i + j];
                        // Positive example: update embeddings with label = 1
                        updateEmbeddings(embeddings, embeddingDimensions, targetNode, contextNode, 1, learningRate);
                        // Negative sampling:
                        std::vector<int> negativeSamples = getNegativeSamples(graph, targetNode, numNegativeSamples);
                        for (int negativeNode : negativeSamples) {
                            updateEmbeddings(embeddings, embeddingDimensions, targetNode, negativeNode, 0, learningRate);
                        }
                    }
                }
            }
        }
    }

    // Sample a set of vectors:
    std::vector<std::vector<double>> getSample(const std::vector<std::vector<double>> &setOfVectors, int sampleSize) {
        std::vector<std::vector<double>> sample;
        if (setOfVectors.empty() || sampleSize <= 0)
            return sample;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, setOfVectors.size() - 1);
        for (int i = 0; i < sampleSize; i++) {
            sample.push_back(setOfVectors[dis(gen)]);
        }
        return sample;
    }

    // Find the k-most similar nodes based on cosine similarity:
    std::vector<std::vector<double>> getSimilarNodes(
        const std::unordered_map<int, std::vector<double>> &embeddings,
        const std::vector<double> &queryVector,
        int kSimilarNodes) {
        if (embeddings.empty() || queryVector.empty() || kSimilarNodes <= 0)
            return {};
        double sumQuery = std::accumulate(queryVector.begin(), queryVector.end(), 0.0,
                                          [](double acc, double v) { return acc + v * v; });
        double normQuery = std::sqrt(sumQuery);
        if (normQuery == 0)
            return {};
        using SimilarityPair = std::pair<double, int>;
        std::priority_queue<SimilarityPair, std::vector<SimilarityPair>, std::greater<>> minHeap;
        for (const auto &[nodeID, embedding] : embeddings) {
            if (embedding.size() != queryVector.size() || embedding == queryVector)
                continue;
            double dot = 0.0, sumCandidate = 0.0;
            for (size_t i = 0; i < embedding.size(); ++i) {
                dot += queryVector[i] * embedding[i];
                sumCandidate += embedding[i] * embedding[i];
            }
            double normCandidate = std::sqrt(sumCandidate);
            if (normCandidate == 0)
                continue;
            double cosineSimilarity = dot / (normQuery * normCandidate);
            if (minHeap.size() < static_cast<size_t>(kSimilarNodes)) {
                minHeap.emplace(cosineSimilarity, nodeID);
            } else if (cosineSimilarity > minHeap.top().first) {
                minHeap.pop();
                minHeap.emplace(cosineSimilarity, nodeID);
            }
        }
        std::vector<std::vector<double>> topKSimilar;
        while (!minHeap.empty()) {
            int nodeID = minHeap.top().second;
            minHeap.pop();
            topKSimilar.push_back(embeddings.at(nodeID));
        }
        std::reverse(topKSimilar.begin(), topKSimilar.end());
        return topKSimilar;
    }

    
    void updateEmbeddings(std::unordered_map<int, std::vector<double>> &embeddings,
                          int dimensions, int targetNode, int contextNode,
                          double label, double lr) {
        std::vector<double> &targetVec = embeddings[targetNode];
        std::vector<double> &contextVec = embeddings[contextNode];
        double dot = std::inner_product(targetVec.begin(), targetVec.end(), contextVec.begin(), 0.0);
        double score = 1.0 / (1.0 + std::exp(-dot)); // sigmoid function
        double gradient = (label - score) * lr;
        for (int i = 0; i < dimensions; ++i) {
            double temp = targetVec[i];
            targetVec[i] += gradient * contextVec[i];
            contextVec[i] += gradient * temp;
        }
    }

    std::vector<int> getNegativeSamples(std::shared_ptr<Graph> graph, int excludeNode, int numSamples) {
        std::vector<int> negativeSamples;
        std::vector<int> nodes = graph->getNodes();
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(0, nodes.size() - 1);
        while (negativeSamples.size() < static_cast<size_t>(numSamples)) {
            int sampledNode = nodes[dist(gen)];
            if (sampledNode != excludeNode)
                negativeSamples.push_back(sampledNode);
        }
        return negativeSamples;
    }
};

#endif // EMBEDDING_STRATEGY_HPP
