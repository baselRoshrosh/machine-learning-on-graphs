#include <gtest/gtest.h>
#include <vector>
#include <unordered_map>
#include <memory>
#include <random>

#include "Topo2Vec.hpp"

using namespace std;

// Public Wrapper Class for Testing
class TestableTopo2Vec : public Topo2Vec
{
public:
    TestableTopo2Vec() = default;
    TestableTopo2Vec(shared_ptr<Graph> graph) : Topo2Vec(graph) {}

    // expose protected methods
    using Topo2Vec::createEmbeddings;
    using Topo2Vec::expandSubgraph;
    using Topo2Vec::getContextSubgraphs;
};

class Topo2VecTest : public ::testing::Test
{
protected:
    shared_ptr<Graph> graph;
    unique_ptr<TestableTopo2Vec> topo2vec; // Use a pointer instead of a direct object

    void SetUp() override
    {
        graph = make_shared<Graph>("../input/cornell/cornell_features.txt", "../input/cornell/cornell_edges.txt");
        topo2vec = make_unique<TestableTopo2Vec>(graph); // Initialize properly
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
    vector<int> templist = {104, 121};
    unordered_map<int, bool> visited;
    for (int nodeID : graph->getNodes())
    {
        visited[nodeID] = false;
    }

    unordered_set<int> subgraphNodes({104, 121});
    int edgesCount = 0;

    topo2vec->expandSubgraph(templist, visited, subgraphNodes, edgesCount);

    EXPECT_GT(edgesCount, 0); // Ensure edges are added
}
