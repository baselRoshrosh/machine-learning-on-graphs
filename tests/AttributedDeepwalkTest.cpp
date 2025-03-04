#include <gtest/gtest.h>
#include "Graph.hpp"
#include "AttributedDeepwalk.hpp"

using namespace std;

// Public Wrapper Class for Testing
class TestableAttributedDeepwalk : public AttributedDeepwalk
{
public:
    TestableAttributedDeepwalk() = default;
    TestableAttributedDeepwalk(shared_ptr<Graph> graph) : AttributedDeepwalk(graph) {}

    // expose protected methods
    using AttributedDeepwalk::calculateWeightMatrix;
    using AttributedDeepwalk::getAliasTables;
    using AttributedDeepwalk::measuring_attribute_similarity;
    using AttributedDeepwalk::measuring_structural_similarity;
};

class AttributedDeepwalkTest : public ::testing::Test
{
protected:
    shared_ptr<Graph> graph;
    unique_ptr<TestableAttributedDeepwalk> adw; // Use a pointer instead of a direct object

    void SetUp() override
    {
        graph = make_shared<Graph>("../input/cornell/cornell_features.txt", "../input/cornell/cornell_edges.txt");
        adw = make_unique<TestableAttributedDeepwalk>(graph); // Initialize properly
    }
};

TEST_F(AttributedDeepwalkTest, CalculateWeightMatrix)
{
    // Calculate weight matrix
    adw->calculateWeightMatrix();

    // Validate the weights (expected values depend on feature similarity and structure)
    auto edges = graph->getEdges();

    double weight = graph->getEdgeWeight(edges[0].first, edges[0].second);

    EXPECT_GT(weight, 0.0); // Ensure weight is positive
}

TEST_F(AttributedDeepwalkTest, GetAliasTables)
{
    auto aliasTables = adw->getAliasTables();

    // Ensure alias table is created for node 1
    for (int nodeID : graph->getNodes())
    {
        ASSERT_TRUE(aliasTables.find(nodeID) != aliasTables.end());
    }

    // Verify alias table properties
    for (int nodeID : graph->getNodes())
    {
        auto neighbors = graph->getNeighbors(nodeID);
        auto aliasTable = aliasTables[nodeID];

        EXPECT_EQ(aliasTable.size(), neighbors.size()); // Should match the number of neighbors
    }
}

TEST_F(AttributedDeepwalkTest, MeasureStructuralSimilarity) {
    double similaritySameNode = adw->measuring_structural_similarity(1, 1);

    EXPECT_EQ(similaritySameNode, 1.0); // node should be similar to itself

    double similarityDifferentNodes = adw->measuring_structural_similarity(1, 2);

    EXPECT_GT(similarityDifferentNodes, 0.0); // similarity should be bigger than 0
    EXPECT_GT(1.0, similarityDifferentNodes); // but smaller then 1
}

TEST_F(AttributedDeepwalkTest, MeasureAttributeSimilarity) {
    double similaritySameNode = adw->measuring_attribute_similarity(1, 1);

    EXPECT_EQ(similaritySameNode, 1.0); // node should be similar to itself

    double similarityDifferentNodes = adw->measuring_attribute_similarity(1, 2);

    EXPECT_GT(similarityDifferentNodes, 0.0); // similarity should be bigger than 0
    EXPECT_GT(1.0, similarityDifferentNodes); // but smaller then 1
}