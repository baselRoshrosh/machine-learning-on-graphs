#include <gtest/gtest.h>
#include <vector>
#include <unordered_map>
#include <memory>
#include <random>

#include "Topo2Vec.hpp"

// Public Wrapper Class for Testing
class TestableTopo2Vec : public Topo2Vec
{
public:
    TestableTopo2Vec() = default;
    TestableTopo2Vec(std::shared_ptr<Graph> graph) : Topo2Vec(graph) {}

    // expose protected methods
    using Topo2Vec::createEmbeddings;
    using Topo2Vec::expandSubgraph;
    using Topo2Vec::getContextSubgraphs;
    using Topo2Vec::getSample;
    using Topo2Vec::skipGram;
};

class Topo2VecTest : public ::testing::Test
{
protected:
    std::shared_ptr<Graph> graph;
    std::unique_ptr<TestableTopo2Vec> topo2vec; // Use a pointer instead of a direct object

    void SetUp() override
    {
        graph = std::make_shared<Graph>("../input/cornell/cornell_features.txt", "../input/cornell/cornell_edges.txt");
        topo2vec = std::make_unique<TestableTopo2Vec>(graph); // Initialize properly
    }
};

/*
 * ======= createEmbeddings() Tests ===================
 */

// Test `createEmbeddings` function
TEST_F(Topo2VecTest, CreateEmbeddingsTest)
{
    int dimensions = 128;

    auto embeddings = topo2vec->createEmbeddings(dimensions);

    EXPECT_GT(embeddings.size(), 0); // Ensure some embeddings are created
    for (const auto &[node, embedding] : embeddings)
    {
        EXPECT_EQ(embedding.size(), dimensions); // Embeddings should match specified dimensions
    }
}

// Test `getContextSubgraphs`
TEST_F(Topo2VecTest, GetContextSubgraphsTest)
{
    auto subgraphs = topo2vec->getContextSubgraphs();

    EXPECT_GT(subgraphs.size(), 0); // There should be some subgraphs
}

// Test `expandSubgraph`
TEST_F(Topo2VecTest, ExpandSubgraphTest)
{   
    std::vector<int> templist = {1, 2};
    std::unordered_map<int, bool> visited;
    for (int nodeID : graph->getNodes()) {
        visited[nodeID] = false;
    }
    visited[1] = true;
    visited[2] = true;

    std::unordered_set<int> subgraphNodes({1, 2});
    int edgesCount = 0;

    std::unordered_set<int> subgraphNodes({1, 2});
    int edgesCount = 0;

    topo2vec->expandSubgraph(templist, visited, subgraphNodes, edgesCount);

    EXPECT_GT(edgesCount, 0); // Ensure edges are added
}

// Test `skipGram`
TEST_F(Topo2VecTest, SkipGramTest)
{   
    int dimensions = 128;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(-0.5 / dimensions, 0.5 / dimensions);
    std::unordered_map<int, std::vector<double>> embeddings;
    for (int nodeID : graph->getNodes()) {
        std::vector<double> vec(dimensions);
        for (double &val : vec)
        {
            val = dist(gen);
        }

        embeddings[nodeID] = vec;
    }

    std::vector<std::vector<int>> subGraphs = {{1, 2}, {2, 3}};

    topo2vec->skipGram(embeddings, subGraphs);

    for (const auto &[node, embedding] : embeddings)
    {
        EXPECT_EQ(embedding.size(), dimensions); // Ensure embedding size remains consistent
    }
}

/*
 * ========= getSample() Tests ==================
 */

// Test 1: Basic Sampling Test
TEST_F(Topo2VecTest, BasicSampling)
{
    TestableTopo2Vec topo;
    std::vector<std::vector<double>> vectors = {
        {1.0, 2.0}, {3.0, 4.0}, {5.0, 6.0}, {7.0, 8.0}};

    int sampleSize = 2;
    auto sample = topo.getSample(vectors, sampleSize);

    EXPECT_EQ(sample.size(), sampleSize); // Ensure correct sample size
    for (const auto &vec : sample)
    {
        EXPECT_TRUE(std::find(vectors.begin(), vectors.end(), vec) != vectors.end());
    }
}

// Test 2: Empty Input Test
TEST_F(Topo2VecTest, EmptyInput)
{
    std::vector<std::vector<double>> vectors;

    auto sample = topo2vec->getSample(vectors, 3);
    EXPECT_TRUE(sample.empty());
}

// Test 3: Sample Size Larger Than Set
TEST_F(Topo2VecTest, SampleSizeLargerThanSet)
{
    std::vector<std::vector<double>> vectors = {
        {1.0, 2.0}, {3.0, 4.0}};

    int sampleSize = 5; // Requesting more samples than available vectors
    auto sample = topo2vec->getSample(vectors, sampleSize);

    EXPECT_EQ(sample.size(), sampleSize); // Function allows duplicates
    for (const auto &vec : sample)
    {
        EXPECT_TRUE(std::find(vectors.begin(), vectors.end(), vec) != vectors.end());
    }
}

// Test 4: Zero and Negative Sample Size Test
TEST_F(Topo2VecTest, ZeroOrNegativeSampleSize)
{
    std::vector<std::vector<double>> vectors = {
        {1.0, 2.0}, {3.0, 4.0}};

    auto sampleZero = topo2vec->getSample(vectors, 0);
    auto sampleNegative = topo2vec->getSample(vectors, -3);

    EXPECT_TRUE(sampleZero.empty());
    EXPECT_TRUE(sampleNegative.empty());
}