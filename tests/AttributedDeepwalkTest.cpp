#include <gtest/gtest.h>
#include "Graph.hpp"
#include "AttributedDeepwalk.hpp"

// Public Wrapper Class for Testing
class TestableAttributedDeepwalk : public AttributedDeepwalk
{
public:
    TestableAttributedDeepwalk() = default;
    TestableAttributedDeepwalk(std::shared_ptr<Graph> graph) : AttributedDeepwalk(graph) {}

    // expose protected methods
    using AttributedDeepwalk::calculateWeightMatrix;
    using AttributedDeepwalk::getAliasTables;
    using AttributedDeepwalk::measuring_attribute_similarity;
    using AttributedDeepwalk::measuring_structural_similarity;
    using AttributedDeepwalk::randomWalk;
    using AttributedDeepwalk::csadw; // <-- Expose the new csadw() method for testing


    int getWalkLength() const { return walkLength; }
    int getEmbeddingDimensions() const { return embeddingDimensions; } // from EmbeddingStrategy

};

class AttributedDeepwalkTest : public ::testing::Test
{
protected:
    std::shared_ptr<Graph> graph;
    std::unique_ptr<TestableAttributedDeepwalk> adw; // Use a pointer instead of a direct object

    void SetUp() override
    {
        graph = std::make_shared<Graph>("../input/cornell/cornell_features.txt", "../input/cornell/cornell_edges.txt");
        adw = std::make_unique<TestableAttributedDeepwalk>(graph); // Initialize properly
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

/*
 * ======= randomWalk() Tests ===================
 */

// Test: `randomWalk()` generates a valid sequence
TEST_F(AttributedDeepwalkTest, RandomWalkGeneratesValidSequence) {
    int startNode = 1;
    std::vector<int> walk = adw->randomWalk(startNode);

    // Check if the first node is the start node
    EXPECT_EQ(walk.front(), startNode);

    // Check if walk length is correct or stops early (if no neighbors)
    EXPECT_LE(walk.size(), adw->getWalkLength());

    // Ensure all nodes in walk exist in the graph
    for (int node : walk) {
        EXPECT_TRUE(std::find(graph->getNodes().begin(), graph->getNodes().end(), node) != graph->getNodes().end());
    }
}

// Test: `randomWalk()` stops early if the node has no neighbors
TEST_F(AttributedDeepwalkTest, RandomWalkStopsIfNoNeighbors) {
    int isolatedNode = -1; // A node ID that is NOT in the graph
    std::vector<int> walk = adw->randomWalk(isolatedNode);

    // Walk should only contain the start node since it has no neighbors
    EXPECT_EQ(walk.size(), 1);
}

// Test: `randomWalk()` only picks valid neighbors
TEST_F(AttributedDeepwalkTest, RandomWalkPicksValidNeighbors) {
    int startNode = 2;
    std::vector<int> walk = adw->randomWalk(startNode);

    // Check each step only moves to a valid neighbor
    for (size_t i = 0; i < walk.size() - 1; ++i) {
        int current = walk[i];
        int next = walk[i + 1];
        std::vector<int> neighbors = graph->getNeighbors(current);
        EXPECT_TRUE(std::find(neighbors.begin(), neighbors.end(), next) != neighbors.end());
    }
}

/*
 * ======= csadw() Test ===================
 */
TEST_F(AttributedDeepwalkTest, CSADWGeneratesValidEmbeddings)
{
    std::cout << "[DEBUG] Running csadw()..." << std::endl;
    
    // Run the method that generates embeddings
    auto embeddings = adw->csadw();

    std::cout << "[DEBUG] Finished csadw(). Embeddings map size: " << embeddings.size() << std::endl;

    // Check if embeddings map is empty
    if (embeddings.empty()) {
        std::cerr << "[ERROR] Embeddings map is unexpectedly empty." << std::endl;
    }
    EXPECT_FALSE(embeddings.empty()) << "Embeddings map is unexpectedly empty.";

    int expectedDim = adw->getEmbeddingDimensions();

    for (int node : graph->getNodes())
    {
        auto it = embeddings.find(node);
        
        if (it == embeddings.end()) {
            std::cerr << "[ERROR] No embedding found for node " << node << std::endl;
        } else {
            std::cout << "[DEBUG] Node " << node << " has embedding size: " << it->second.size() << std::endl;
        }

        EXPECT_NE(it, embeddings.end()) 
            << "No embedding found for node " << node << ".";

        if (it != embeddings.end()) {
            EXPECT_EQ(it->second.size(), static_cast<size_t>(expectedDim))
                << "Embedding for node " << node << " has incorrect dimension.";
        }
    }

    // Check if all embeddings are zero (unexpected)
    bool allZero = true;
    for (const auto &pair : embeddings)
    {
        for (double val : pair.second)
        {
            if (val != 0.0)
            {
                allZero = false;
                break;
            }
        }
        if (!allZero) break;
    }
    if (allZero) {
        std::cerr << "[ERROR] All embeddings are zero, suggesting no training occurred." << std::endl;
    }
    EXPECT_FALSE(allZero) << "All embeddings are zero, suggesting no training occurred.";
}
