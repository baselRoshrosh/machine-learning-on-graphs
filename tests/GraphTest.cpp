#include <gtest/gtest.h>
#include "../src/Graph.tpp"
#include "../include/INode.hpp"
#include "../include/IEdges.hpp"
#include "../include/BasicEdges.hpp"
#include <fstream>

// Helper function to create temporary test files
void createTempFile(const std::string &filename, const std::string &content) {
    std::ofstream outFile(filename);
    outFile << content;
    outFile.close();
}

// Fixture class for Graph testing
class GraphTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create temporary node and edge files
        createTempFile("test_nodes.txt", "1\t1.0,2.0,3.0\t0\n2\t4.0,5.0,6.0\t1");
        createTempFile("test_edges.txt", "1 2\n2 1");

        // Initialize Graph
        graph = new Graph<double>("test_nodes.txt", "test_edges.txt");
    }

    void TearDown() override {
        delete graph;
        std::remove("test_nodes.txt");
        std::remove("test_edges.txt");
    }

    Graph<double> *graph;
};

// Test Graph Initialization
TEST_F(GraphTest, Initialization) {
    EXPECT_EQ(graph->getNodeCount(), 2);
    EXPECT_EQ(graph->getEdgeCount(), 1); // edges are undirected
}

// Test Node Retrieval
TEST_F(GraphTest, GetNodes) {
    std::vector<int> expectedNodes = {1, 2};
    EXPECT_EQ(graph->getNodes(), expectedNodes);
}

// Test Edge Retrieval
TEST_F(GraphTest, GetEdges) {
    std::vector<std::pair<int, int>> expectedEdges = {{1, 2}}; // Adjust for undirected graph
    EXPECT_EQ(graph->getEdges(), expectedEdges);
}


// Test Neighbor Retrieval
TEST_F(GraphTest, GetNeighbors) {
    std::vector<int> neighbors = graph->getNeighbors(1);
    EXPECT_EQ(neighbors.size(), 1);
    EXPECT_EQ(neighbors[0], 2);
}

// Test Node and Edge Count
TEST_F(GraphTest, NodeEdgeCount) {
    EXPECT_EQ(graph->getNodeCount(), 2);
    EXPECT_EQ(graph->getEdgeCount(), 1); // undirected graph
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
