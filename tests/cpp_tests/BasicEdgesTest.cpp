#include <gtest/gtest.h>
#include "BasicEdges.hpp"
#include "Graph.hpp"

#include <cmath> // for isnan
#include <memory>

using namespace std;

const string NODES_FILE = "../input/cornell/cornell_mcar_0.5.txt";
const string EDGE_FILE = "../input/cornell/cornell_edges.txt";

// Test fixture for BasicEdges
class BasicEdgesTest : public ::testing::Test
{
protected:
    shared_ptr<Graph> graph;
    unique_ptr<BasicEdges> edges;

    void SetUp() override
    {
        // Load the real graph
        graph = make_shared<Graph>(NODES_FILE, EDGE_FILE);
        edges = make_unique<BasicEdges>(graph->getEdges()); // Initialize BasicEdges with dataset
    }
};

// Test: Check neighbors retrieval
TEST_F(BasicEdgesTest, GetNeighbors)
{
   // Choose a node from the dataset
   int testNode = 1; // Adjust based on known graph structure
   vector<int> neighbors = edges->getNeighbors(testNode);

   // Validate that neighbors exist
   EXPECT_FALSE(neighbors.empty()) << "Node " << testNode << " should have neighbors.";

   // Validate against the real dataset
   vector<int> expectedNeighbors = graph->getNeighbors(testNode);
   EXPECT_EQ(neighbors, expectedNeighbors);
}

// Test: Check edge existence
TEST_F(BasicEdgesTest, IsEdge)
{
    auto realEdges = graph->getEdges();

    // Pick the first edge from the dataset to test
    if (!realEdges.empty())
    {
        int u = realEdges[0].first;
        int v = realEdges[0].second;

        EXPECT_TRUE(edges->isEdge(u, v)) << "Edge (" << u << ", " << v << ") should exist.";
        EXPECT_TRUE(edges->isEdge(v, u)) << "Graph is undirected, (" << v << ", " << u << ") should exist.";
    }

    // Check for an edge that shouldn't exist
    EXPECT_FALSE(edges->isEdge(999, 1000)) << "Edge (999, 1000) should not exist.";
}

// Test: Retrieve all edges
TEST_F(BasicEdgesTest, GetEdges)
{
    EXPECT_EQ(edges->getEdges(), graph->getEdges()) << "Edges should match the dataset.";
}

// Test case: Test adding an edge to the BasicEdges object
TEST_F(BasicEdgesTest, AddEdge)
{
    size_t initialSize = edges->getEdges().size();

    // Pick two nodes that don't already have an edge
    int node1 = 5, node2 = 100;
    EXPECT_FALSE(edges->isEdge(node1, node2));

    edges->addEdge(node1, node2);

    size_t currentSize = edges->getEdges().size();
    EXPECT_EQ(currentSize, initialSize + 1) << "Edge count should increase by 1.";

    // Check if the new edge exists
    EXPECT_TRUE(edges->isEdge(node1, node2)) << "New edge should exist.";
}

// Test: getting Edge Weight
TEST_F(BasicEdgesTest, GetEdgeWeight)
{
    auto realEdges = graph->getEdges();

    if (!realEdges.empty())
    {
        int u = realEdges[0].first;
        int v = realEdges[0].second;
        double weight = edges->getWeight(u, v);

        EXPECT_TRUE(isnan(weight) || weight >= 0.0) << "Edge weight should be NaN (if unweighted) or positive.";
    }
}

// Test: setting Edge Weight
TEST_F(BasicEdgesTest, SetEdgeWeight)
{
    double weight = 0.5;
    int node1 = 173, node2 = 96; // Pick nodes from the dataset
    EXPECT_TRUE(edges->isEdge(node1, node2));
    edges->setWeight(node1, node2, weight);
    EXPECT_EQ(edges->getWeight(node1, node2), weight) << "Weight should be updated.";
}
