#include <gtest/gtest.h>
#include <vector>
#include <unordered_map>
#include <random>
#include <algorithm>

// Include our strategy header and Graph header
#include "EmbeddingStrategy.hpp"
#include "Graph.hpp"

// Public wrapper class to expose protected members
class TestableEmbeddingStrategy : public EmbeddingStrategy
{
public:
    // Constructor that takes a valid graph pointer and sets the protected member.
    TestableEmbeddingStrategy(std::shared_ptr<Graph> graphPtr)
    {
        this->graph = graphPtr;
    }
    // Implement the pure virtual methods from IStrategies.
    void run() override {}
    std::shared_ptr<Graph> extractResults() const override { return graph; }
    void configure(const std::map<std::string, double> &params) override {}
    void reset() override {}

    // Expose the protected methods for testing.
    using EmbeddingStrategy::getSample;
    using EmbeddingStrategy::getSimilarNodes;
    using EmbeddingStrategy::skipGram;
};

class EmbeddingStrategyTest : public ::testing::Test
{
protected:
    std::unique_ptr<TestableEmbeddingStrategy> embeddingStrategy;
    std::shared_ptr<Graph> graph;

    // In SetUp, we initialize our strategy with a valid graph.
    // (Here we use the real Cornell files—adjust the paths if needed.)
    void SetUp() override
    {
        graph = std::make_shared<Graph>("../input/cornell/cornell_features.txt",
                                        "../input/cornell/cornell_edges.txt");
        embeddingStrategy = std::make_unique<TestableEmbeddingStrategy>(graph);
    }
};

/*
 * ======= skipGram() Tests ===================
 */
TEST_F(EmbeddingStrategyTest, SkipGramTest)
{
    int dimensions = 128;

    // Create random embeddings for all nodes in the graph.
    std::unordered_map<int, std::vector<double>> embeddings;
    auto nodes = graph->getNodes();
    for (int nodeID : nodes)
    {
        std::vector<double> vec(dimensions);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> dist(-0.5 / dimensions, 0.5 / dimensions);
        for (double &val : vec)
            val = dist(gen);
        embeddings[nodeID] = vec;
    }

    // For testing skipGram, we need to provide a set of context graphs.
    // Here, we simply take a small subset from our graph.
    std::vector<int> subset;
    if (nodes.size() >= 4)
    {
        subset = {nodes[0], nodes[1], nodes[2], nodes[3]};
    }
    else
    {
        subset = nodes;
    }
    // Wrap the subset in a vector (we can test with one context graph).
    std::vector<std::vector<int>> subGraphs = {subset};

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
    std::unordered_map<int, std::vector<double>> embeddings = {
        {1, {1.0, 2.0}}, {2, {3.0, 4.0}}, {3, {5.0, 6.0}}, {4, {7.0, 8.0}}};
    int sampleSize = 2;
    auto sample = embeddingStrategy->getSample(embeddings, sampleSize);
    EXPECT_EQ(sample.size(), static_cast<size_t>(sampleSize));
    for (const auto &vec : sample)
    {
        EXPECT_TRUE(std::find(embeddings.begin(), embeddings.end(), vec) != embeddings.end());
    }
}

TEST_F(EmbeddingStrategyTest, EmptyInput)
{
    std::unordered_map<int, std::vector<double>> embeddings;
    auto sample = embeddingStrategy->getSample(embeddings, 3);
    EXPECT_TRUE(sample.empty());
}

TEST_F(EmbeddingStrategyTest, SampleSizeLargerThanSet)
{
    std::unordered_map<int, std::vector<double>> embeddings = {
        {1, {1.0, 2.0}}, {2, {3.0, 4.0}}};

    int sampleSize = 5; // duplicates allowed
    auto sample = embeddingStrategy->getSample(embeddings, sampleSize);
    EXPECT_EQ(sample.size(), static_cast<size_t>(sampleSize));
    for (const auto &vec : sample)
    {
        EXPECT_TRUE(std::find(embeddings.begin(), embeddings.end(), vec) != embeddings.end());
    }
}

TEST_F(EmbeddingStrategyTest, ZeroOrNegativeSampleSize)
{
    std::unordered_map<int, std::vector<double>> embeddings = {
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
    std::unordered_map<int, std::vector<double>> embeddings = {
        {1, {0.1, 0.2, 0.3}},
        {2, {0.2, 0.1, 0.4}},
        {3, {0.3, 0.2, 0.1}},
        {4, {0.1, 0.0, 0.3}}};
    std::vector<double> queryVector = {0.1, 0.2, 0.3};
    auto similarNodes = embeddingStrategy->getSimilarNodes(embeddings, queryVector, 2);
    // Ensure that none of the returned vectors exactly match the query vector.
    for (const auto &vec : similarNodes)
    {
        EXPECT_NE(vec, queryVector);
    }
    EXPECT_EQ(similarNodes.size(), 2);
}
