#include <gtest/gtest.h>
#include "BasicEdges.hpp"

#include <cmath> // for std::isnan

// Test fixture for BasicEdges
class AdjacencyArrayTest : public ::testing::Test
{
protected:
    // Test data
    std::vector<std::pair<int, int>> initialEdges = {{1, 2}, {2, 3}, {3, 4}};
    BasicEdges edges;

    // Set up the test environment
    AdjacencyArrayTest() : edges(initialEdges) {}

    // Tear down after each test (if needed)
    void TearDown() override
    {
        // Cleanup code if necessary
    }
};

// Test: Check neighbors retrieval
TEST_F(AdjacencyArrayTest, GetNeighbors)
{
    // Check neighbors for node 2
    std::vector<int> neighbors = edges.getNeighbors(2);
    std::vector<int> expected = {1, 3};
    EXPECT_EQ(neighbors, expected);

    // Check neighbors for node 4
    neighbors = edges.getNeighbors(4);
    expected = {3};
    EXPECT_EQ(neighbors, expected);

    // Check neighbors for a node with no edges
    neighbors = edges.getNeighbors(5);
    EXPECT_TRUE(neighbors.empty());
}

// Test: Check edge existence
TEST_F(AdjacencyArrayTest, IsEdge)
{
    // Existing edges
    EXPECT_TRUE(edges.isEdge(1, 2));
    EXPECT_TRUE(edges.isEdge(3, 4));

    // Non-existing edges
    EXPECT_FALSE(edges.isEdge(1, 4));
    EXPECT_FALSE(edges.isEdge(5, 6));

    // Reversed edges (since graph is undirected)
    EXPECT_TRUE(edges.isEdge(2, 1));
    EXPECT_TRUE(edges.isEdge(4, 3));
}

// Test: Retrieve all edges
TEST_F(AdjacencyArrayTest, GetEdges)
{
    const auto &actualEdges = edges.getEdges();
    EXPECT_EQ(actualEdges, initialEdges); // Ensure edges match the initialization data

    // Check immutability of the returned edges
    // Uncommenting the following line should cause a compilation error:
    // actualEdges.push_back({5, 6});
}

// Test case: Test adding an edge to the BasicEdges object
TEST_F(BasicEdgesTest, AddEdge)
{
    // Initial edges before adding the new edge
    std::vector<std::pair<int, int>> initialEdges = edges.getEdges();
    size_t initialSize = initialEdges.size();

    // Add a new edge (3, 4)
    edges.addEdge(3, 4);

    // Retrieve the edges after adding the new one
    std::vector<std::pair<int, int>> currentEdges = edges.getEdges();
    size_t currentSize = currentEdges.size();

    // Assert that the size of edges increased by 1 (i.e., the new edge was added)
    EXPECT_EQ(currentSize, initialSize + 1);

    // Check that the new edge (3, 4) is in the list of edges
    EXPECT_NE(std::find(currentEdges.begin(), currentEdges.end(), std::make_pair(3, 4)), currentEdges.end());
}

// Test: getting Edge Weight
TEST_F(BasicEdgesTest, GetEdgeWeight)
{
    double weight = edges.getWeight(1, 2);

    EXPECT_TRUE(std::isnan(weight)); // This will pass if weight is NaN
}

// Test: setting Edge Weight
TEST_F(BasicEdgesTest, SetEdgeWeight)
{
    double weight = 0.5;

    edges.setWeight(1, 2, weight);

    EXPECT_EQ(edges.getWeight(1, 2), weight);
}
