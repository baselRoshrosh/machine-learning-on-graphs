#include <gtest/gtest.h>
#include <algorithm>

#include "AdjacencyArrayEdges.hpp"
#include "Graph.hpp"

const std::string NODES_FILE = "../input/cornell/cornell_mcar_0.5.txt";
const std::string EDGE_FILE = "../input/cornell/cornell_edges.txt";

// Test fixture for BasicEdges
class AdjacencyArrayEdgesTest : public ::testing::Test
{
protected:
    Graph graph;  // Graph object to load edges
    AdjacencyArrayEdges edges;

    // Set up the test environment
    AdjacencyArrayEdgesTest() : graph(NODES_FILE, EDGE_FILE), edges(graph.getEdges()) {}
};

// Test: Check neighbors retrieval
TEST_F(AdjacencyArrayEdgesTest, GetNeighbors)
{
    // Pick a node that has known neighbors in the dataset
    int testNode = 1;
    std::vector<int> neighbors = edges.getNeighbors(testNode);

    // Ensure neighbors list is not empty
    EXPECT_FALSE(neighbors.empty());

    // Check that all neighbors are valid nodes in the graph
    auto allNodes = graph.getNodes();
    for (int neighbor : neighbors)
    {
        EXPECT_TRUE(std::find(allNodes.begin(), allNodes.end(), neighbor) != allNodes.end());
    }
}

// Test: Check edge existence
TEST_F(AdjacencyArrayEdgesTest, IsEdge)
{
    auto edgesList = graph.getEdges();
    if (!edgesList.empty())
    {
        auto [node1, node2] = edgesList[0]; // Take the first edge from dataset
        EXPECT_TRUE(edges.isEdge(node1, node2));
        EXPECT_TRUE(edges.isEdge(node2, node1)); // Since graph is undirected
    }

    // Check a non-existing edge (Pick arbitrary nodes not in dataset)
    EXPECT_FALSE(edges.isEdge(500, 600));
}

// Test: Retrieve all edges
TEST_F(AdjacencyArrayEdgesTest, GetEdges)
{
    const auto &actualEdges = edges.getEdges();
    const auto &graphEdges = graph.getEdges();

    // Ensure that AdjacencyArrayEdges contains the same edges as Graph
    EXPECT_EQ(actualEdges, graphEdges);
}

// Test case: Test adding an edge to the BasicEdges object
TEST_F(AdjacencyArrayEdgesTest, AddEdge)
{
    size_t initialSize = edges.getEdges().size();

    // Choose two arbitrary nodes
    int nodeA = 10;
    int nodeB = 20;
    
    // Ensure edge does not exist before adding
    EXPECT_FALSE(edges.isEdge(nodeA, nodeB));

    // Add the new edge
    edges.addEdge(nodeA, nodeB);

    // Ensure edge exists after adding
    EXPECT_TRUE(edges.isEdge(nodeA, nodeB));

    // Ensure the edge list size increased
    EXPECT_EQ(edges.getEdges().size(), initialSize + 1);
}
