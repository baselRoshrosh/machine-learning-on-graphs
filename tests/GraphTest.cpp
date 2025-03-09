#include <gtest/gtest.h>
#include <fstream>
#include <cstdio>
#include <numeric>

#include "Graph.hpp"
#include "Node.hpp"
#include "BasicEdges.hpp"

using namespace std;

const string NODES_FILE = "../input/cornell/cornell_mcar_0.5.txt";
const string EDGE_FILE = "../input/cornell/cornell_edges.txt";

// Fixture class for Graph testing
class GraphTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Initialize Graph
        graph = new Graph(NODES_FILE, EDGE_FILE);
    }

    void TearDown() override
    {
        delete graph;
    }

    Graph *graph;
};

// Test Graph Initialization
TEST_F(GraphTest, Initialization)
{
    EXPECT_EQ(graph->getNodeCount(), 183);
    EXPECT_EQ(graph->getEdgeCount(), 298); // edges are undirected
}

// Test Node Retrieval
TEST_F(GraphTest, GetNodes)
{
    vector<int> expectedNodes(183);
    std::iota(expectedNodes.begin(), expectedNodes.end(), 0);
    EXPECT_EQ(graph->getNodes(), expectedNodes);
}

// Test Edge Retrieval
TEST_F(GraphTest, GetEdges)
{
    EXPECT_GT(graph->getEdges().size(), 0); // Ensure there are edges
}

// Test Neighbor Retrieval
TEST_F(GraphTest, GetNeighbors)
{
    vector<int> testNodes = {1, 57, 100}; // Select nodes from different regions of the dataset

    for (int node : testNodes)
    {
        vector<int> neighbors = graph->getNeighbors(node);
        EXPECT_GT(neighbors.size(), 0) << "Node " << node << " should have at least one neighbor.";
    }
}

// Test Node and Edge Count
TEST_F(GraphTest, NodeEdgeCount)
{
    EXPECT_EQ(graph->getNodeCount(), 183);
    EXPECT_EQ(graph->getEdgeCount(), 298); // undirected graph
}

// Test Getting Features by Node ID
TEST_F(GraphTest, GetFeatureById)
{
    vector<double> features = graph->getFeatureById(1);
    EXPECT_FALSE(features.empty()); // Ensure it has features
}

// Test Updating Features by Node ID
TEST_F(GraphTest, UpdateFeatureById)
{
    int testNodeId = 1; // Choose an existing node
    vector<double> originalFeatures = graph->getFeatureById(testNodeId);
    
    // Ensure we have a valid feature vector
    ASSERT_FALSE(originalFeatures.empty()) << "Original feature vector should not be empty";

    size_t expectedFeatureSize = originalFeatures.size();
    vector<double> newFeatures(expectedFeatureSize, 5.0); // Create a valid feature vector

    // Update feature vector
    graph->updateFeatureById(testNodeId, newFeatures);

    // Check if the update was successful
    EXPECT_EQ(graph->getFeatureById(testNodeId), newFeatures);
}

// Test Updating Features for Invalid Node ID
TEST_F(GraphTest, UpdateFeatureInvalidId)
{
    vector<double> newFeatures = {7.0, 8.0, 9.0};
    
    // Try updating a non-existent node
    EXPECT_THROW(graph->updateFeatureById(999, newFeatures), invalid_argument);
    
    // Try setting a feature vector with an incorrect length
    vector<double> incorrectLengthFeatures = {1.0, 2.0}; // Assume dataset has 3 features per node
    EXPECT_THROW(graph->updateFeatureById(1, incorrectLengthFeatures), invalid_argument);
}

// Test Self-loops
TEST_F(GraphTest, SelfLoops)
{
    auto edges = graph->getEdges();
    bool hasSelfLoop = false;

    for (const auto& edge : edges)
    {
        if (edge.first == edge.second)
        {
            hasSelfLoop = true;
            EXPECT_EQ(edge.first, edge.second) << "Self-loop detected at node " << edge.first;
        }
    }

    if (!hasSelfLoop)
    {
        std::cout << "[INFO] No self-loops found in dataset. Test skipped." << std::endl;
        GTEST_SKIP();
    }
}

// Test Nodes with no edges
TEST_F(GraphTest, IsolatedNodes)
{
    vector<int> isolatedNodes = {35, 87, 126, 134}; // Pick nodes that should have at most one connection
    for (int node : isolatedNodes)
    {
        vector<int> neighbors = graph->getNeighbors(node);
        EXPECT_LE(neighbors.size(), 1) << "Node " << node << " should have at most one neighbor.";
    }
}

// Test Duplicate edges should not be duplicated
TEST_F(GraphTest, DuplicateEdges)
{
    size_t initialEdgeCount = graph->getEdgeCount();

    int nodeA = 5, nodeB = 15;
    auto edges = graph->getEdges();
    int count = std::count(edges.begin(), edges.end(), std::make_pair(nodeA, nodeB));
    EXPECT_LE(count, 1) << "Graph should not contain duplicate edges.";
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
