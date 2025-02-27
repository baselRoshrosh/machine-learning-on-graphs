#include <gtest/gtest.h>
#include <vector>
#include <unordered_map>
#include <memory>

#include "AttributedDeepwalk.hpp"

// Public Wrapper Class for Testing
class TestableAttributedDeepwalk : public AttributedDeepwalk {
public:
    TestableAttributedDeepwalk(std::shared_ptr<Graph> graph) : AttributedDeepwalk(graph) {}

    // Expose protected method for testing
    using AttributedDeepwalk::randomWalk;

    // Expose walkLength with a getter function
    int getWalkLength() const { return walkLength; }
};

// Test Fixture for AttributedDeepwalk
class AttributedDeepwalkTest : public ::testing::Test {
protected:
    std::shared_ptr<Graph> graph;
    std::unique_ptr<TestableAttributedDeepwalk> deepwalk;

    void SetUp() override {
        graph = std::make_shared<Graph>("../input/cornell/cornell_features.txt", "../input/cornell/cornell_edges.txt");
        deepwalk = std::make_unique<TestableAttributedDeepwalk>(graph);
    }
};

/*
 * ======= randomWalk() Tests ===================
 */

// Test: `randomWalk()` generates a valid sequence
TEST_F(AttributedDeepwalkTest, RandomWalkGeneratesValidSequence) {
    int startNode = 1;
    std::vector<int> walk = deepwalk->randomWalk(startNode);

    // Check if the first node is the start node
    EXPECT_EQ(walk.front(), startNode);

    // Check if walk length is correct or stops early (if no neighbors)
    EXPECT_LE(walk.size(), deepwalk->getWalkLength());

    // Ensure all nodes in walk exist in the graph
    for (int node : walk) {
        EXPECT_TRUE(std::find(graph->getNodes().begin(), graph->getNodes().end(), node) != graph->getNodes().end());
    }
}

// Test: `randomWalk()` stops early if the node has no neighbors
TEST_F(AttributedDeepwalkTest, RandomWalkStopsIfNoNeighbors) {
    int isolatedNode = -1; // A node ID that is NOT in the graph
    std::vector<int> walk = deepwalk->randomWalk(isolatedNode);

    // Walk should only contain the start node since it has no neighbors
    EXPECT_EQ(walk.size(), 1);
}

// Test: `randomWalk()` only picks valid neighbors
TEST_F(AttributedDeepwalkTest, RandomWalkPicksValidNeighbors) {
    int startNode = 2;
    std::vector<int> walk = deepwalk->randomWalk(startNode);

    // Check each step only moves to a valid neighbor
    for (size_t i = 0; i < walk.size() - 1; ++i) {
        int current = walk[i];
        int next = walk[i + 1];
        std::vector<int> neighbors = graph->getNeighbors(current);
        EXPECT_TRUE(std::find(neighbors.begin(), neighbors.end(), next) != neighbors.end());
    }
}
