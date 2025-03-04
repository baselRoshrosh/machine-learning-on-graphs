#include <gtest/gtest.h>
#include <vector>
#include <unordered_map>
#include <random>
#include <algorithm>

// Include our strategy header and Graph header
#include "EmbeddingStrategy.hpp"
#include "Graph.hpp"

using namespace std;

// Public wrapper class to expose protected members
class TestableEmbeddingStrategy : public EmbeddingStrategy
{
public:
    // Constructor that takes a valid graph pointer and sets the protected member.
    TestableEmbeddingStrategy(shared_ptr<Graph> graphPtr)
    {
        this->graph = graphPtr;
    }
    // Implement the pure virtual methods from IStrategies.
    void run() override {}
    shared_ptr<Graph> extractResults() const override { return graph; }
    void configure(const map<string, double> &params) override {}
    void reset() override {}

    // Expose the protected methods for testing.
    using EmbeddingStrategy::getSample;
    using EmbeddingStrategy::getSimilarNodes;
    using EmbeddingStrategy::skipGram;
};

class EmbeddingStrategyTest : public ::testing::Test
{
protected:
    unique_ptr<TestableEmbeddingStrategy> embeddingStrategy;
    shared_ptr<Graph> graph;

    // In SetUp, we initialize our strategy with a valid graph.
    // (Here we use the real Cornell files—adjust the paths if needed.)
    void SetUp() override
    {
        graph = make_shared<Graph>("../input/cornell/cornell_features.txt",
                                        "../input/cornell/cornell_edges.txt");
        embeddingStrategy = make_unique<TestableEmbeddingStrategy>(graph);
    }
};

/*
 * ======= skipGram() Tests ===================
 */
TEST_F(EmbeddingStrategyTest, SkipGramTest)
{
    int dimensions = 128;

    // Create random embeddings for all nodes in the graph.
    unordered_map<int, vector<double>> embeddings;
    auto nodes = graph->getNodes();
    for (int nodeID : nodes)
    {
        vector<double> vec(dimensions);
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<double> dist(-0.5 / dimensions, 0.5 / dimensions);
        for (double &val : vec)
            val = dist(gen);
        embeddings[nodeID] = vec;
    }

    // For testing skipGram, we need to provide a set of context graphs.
    // Here, we simply take a small subset from our graph.
    vector<int> subset;
    if (nodes.size() >= 4)
    {
        subset = {nodes[0], nodes[1], nodes[2], nodes[3]};
    }
    else
    {
        subset = nodes;
    }
    // Wrap the subset in a vector (we can test with one context graph).
    vector<vector<int>> subGraphs = {subset};

    // Call skipGram to update the embeddings in place.
    embeddingStrategy->skipGram(embeddings, subGraphs);

    // Verify that each embedding still has the correct dimension.
    for (const auto &[node, emb] : embeddings)
    {
        EXPECT_EQ(emb.size(), static_cast<size_t>(dimensions));
    }
}

/*
 * ========= getSample() Tests ===================
 */
TEST_F(EmbeddingStrategyTest, BasicSampling)
{
    unordered_map<int, vector<double>> embeddings = {
        {1, {1.0, 2.0}}, {2, {3.0, 4.0}}, {3, {5.0, 6.0}}, {4, {7.0, 8.0}}};
    int sampleSize = 2;
    auto sample = embeddingStrategy->getSample(embeddings, sampleSize);
    EXPECT_EQ(sample.size(), static_cast<size_t>(sampleSize));
    for (const auto &vec : sample)
    {
        EXPECT_TRUE(find(embeddings.begin(), embeddings.end(), vec) != embeddings.end());
    }
}

TEST_F(EmbeddingStrategyTest, EmptyInput)
{
    unordered_map<int, vector<double>> embeddings;
    auto sample = embeddingStrategy->getSample(embeddings, 3);
    EXPECT_TRUE(sample.empty());
}

TEST_F(EmbeddingStrategyTest, SampleSizeLargerThanSet)
{
    unordered_map<int, vector<double>> embeddings = {
        {1, {1.0, 2.0}}, {2, {3.0, 4.0}}};

    int sampleSize = 5;
    auto sample = embeddingStrategy->getSample(embeddings, sampleSize);
    EXPECT_EQ(sample.size(), min(static_cast<size_t>(sampleSize), embeddings.size()));
    for (const auto &vec : sample)
    {
        EXPECT_TRUE(find(embeddings.begin(), embeddings.end(), vec) != embeddings.end());
    }
}

TEST_F(EmbeddingStrategyTest, ZeroOrNegativeSampleSize)
{
    unordered_map<int, vector<double>> embeddings = {
        {1, {1.0, 2.0}}, {2, {3.0, 4.0}}};
    auto sampleZero = embeddingStrategy->getSample(embeddings, 0);
    auto sampleNegative = embeddingStrategy->getSample(embeddings, -3);
    EXPECT_TRUE(sampleZero.empty());
    EXPECT_TRUE(sampleNegative.empty());
}

/*
 * ========= getSimilarNodes() Tests ===================
 */
TEST_F(EmbeddingStrategyTest, GetSimilarNodes)
{
    unordered_map<int, vector<double>> embeddings = {
        {1, {0.1, 0.2, 0.3}},
        {2, {0.2, 0.1, 0.4}},
        {3, {0.3, 0.2, 0.1}},
        {4, {0.1, 0.0, 0.3}}};
    vector<double> queryVector = {0.1, 0.2, 0.3};
    auto similarNodes = embeddingStrategy->getSimilarNodes(embeddings, queryVector, 2);
    // Ensure that none of the returned vectors exactly match the query vector.
    for (const auto &vec : similarNodes)
    {
        EXPECT_NE(vec, queryVector);
    }
    EXPECT_EQ(similarNodes.size(), 2);
}
