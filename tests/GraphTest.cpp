#include <gtest/gtest.h>
#include <fstream>
#include <cstdio>

#include "Graph.hpp"
#include "Node.hpp"
#include "BasicEdges.hpp"

using namespace std;

// Helper function to create temporary test files
void createTempFile(const string &filename, const string &content)
{
    ofstream outFile(filename);
    outFile << content;
    outFile.close();
}

const string NODES_FILE = "test_nodes.txt";
const string NODES_FILE_INPUT = "1\t1.0,2.0,3.0\t0\n2\t4.0,5.0,6.0\t1";
const string EDGE_FILE = "test_edges.txt";
// Ensure only one direction is stored
const string EDGE_FILE_INPUT = "1 2";

// Fixture class for Graph testing
class GraphTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Create temporary node and edge files
        createTempFile(NODES_FILE, NODES_FILE_INPUT);
        createTempFile(EDGE_FILE, EDGE_FILE_INPUT);

        // Initialize Graph
        graph = new Graph(NODES_FILE, EDGE_FILE);
    }

    void TearDown() override
    {
        delete graph;
        remove(NODES_FILE.c_str());
        remove(EDGE_FILE.c_str());
    }

    Graph *graph;
};

// Test Graph Initialization
TEST_F(GraphTest, Initialization)
{
    EXPECT_EQ(graph->getNodeCount(), 2);
    EXPECT_EQ(graph->getEdgeCount(), 1); // edges are undirected
}

// Test Node Retrieval
TEST_F(GraphTest, GetNodes)
{
    vector<int> expectedNodes = {1, 2};
    EXPECT_EQ(graph->getNodes(), expectedNodes);
}

// Test Edge Retrieval
TEST_F(GraphTest, GetEdges)
{
    vector<pair<int, int>> expectedEdges = {{1, 2}};
    EXPECT_EQ(graph->getEdges(), expectedEdges);
}

// Test Neighbor Retrieval
TEST_F(GraphTest, GetNeighbors)
{
    vector<int> neighbors1 = graph->getNeighbors(1);
    vector<int> neighbors2 = graph->getNeighbors(2);

    EXPECT_EQ(neighbors1.size(), 1);
    EXPECT_EQ(neighbors1[0], 2);

    EXPECT_EQ(neighbors2.size(), 1);
    EXPECT_EQ(neighbors2[0], 1); // Ensure bidirectionality
}

// Test Node and Edge Count
TEST_F(GraphTest, NodeEdgeCount)
{
    EXPECT_EQ(graph->getNodeCount(), 2);
    EXPECT_EQ(graph->getEdgeCount(), 1); // undirected graph
}

// Test Getting Features by Node ID
TEST_F(GraphTest, GetFeatureById)
{
    vector<double> expectedFeatures = {1.0, 2.0, 3.0};
    EXPECT_EQ(graph->getFeatureById(1), expectedFeatures);
}

// Test Updating Features by Node ID
TEST_F(GraphTest, UpdateFeatureById)
{
    vector<double> newFeatures = {7.0, 8.0, 9.0};
    graph->updateFeatureById(1, newFeatures);
    EXPECT_EQ(graph->getFeatureById(1), newFeatures);
}

// Test Updating Features for Invalid Node ID
TEST_F(GraphTest, UpdateFeatureInvalidId)
{
    vector<double> newFeatures = {7.0, 8.0, 9.0};
    EXPECT_THROW(graph->updateFeatureById(99, newFeatures), invalid_argument);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
