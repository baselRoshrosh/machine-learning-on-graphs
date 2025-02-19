#include <gtest/gtest.h>
#include <fstream>
#include <cstdio>

#include "Graph.hpp"
#include "Node.hpp"
#include "BasicEdges.hpp"
#include "KNN.hpp"

// Helper function to create temporary test files
void createTempFile(const std::string &filename, const std::string &content)
{
    std::ofstream outFile(filename);
    outFile << content;
    outFile.close();
}

const std::string NODES_FILE = "test_nodes.txt";
const std::string NODES_FILE_INPUT = "1\t1.0,2.0,3.0\t0\n2\t4.0,5.0,6.0\t1";
const std::string EDGE_FILE = "test_edges.txt";
// Ensure only one direction is stored
const std::string EDGE_FILE_INPUT = "1 2";

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
        std::remove(NODES_FILE.c_str());
        std::remove(EDGE_FILE.c_str());
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
    std::vector<int> expectedNodes = {1, 2};
    EXPECT_EQ(graph->getNodes(), expectedNodes);
}

// Test Edge Retrieval
TEST_F(GraphTest, GetEdges)
{
    std::vector<std::pair<int, int>> expectedEdges = {{1, 2}};
    EXPECT_EQ(graph->getEdges(), expectedEdges);
}

// Test Neighbor Retrieval
TEST_F(GraphTest, GetNeighbors)
{
    std::vector<int> neighbors1 = graph->getNeighbors(1);
    std::vector<int> neighbors2 = graph->getNeighbors(2);

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
    std::vector<double> expectedFeatures = {1.0, 2.0, 3.0};
    EXPECT_EQ(graph->getFeatureById(1), expectedFeatures);
}

// Test Updating Features by Node ID
TEST_F(GraphTest, UpdateFeatureById)
{
    std::vector<double> newFeatures = {7.0, 8.0, 9.0};
    graph->updateFeatureById(1, newFeatures);
    EXPECT_EQ(graph->getFeatureById(1), newFeatures);
}

// Test Updating Features for Invalid Node ID
TEST_F(GraphTest, UpdateFeatureInvalidId)
{
    std::vector<double> newFeatures = {7.0, 8.0, 9.0};
    EXPECT_THROW(graph->updateFeatureById(99, newFeatures), std::invalid_argument);
}

// Test saving features after KNN interpretation
TEST_F(GraphTest, SaveFeaturesAfterKNNInterpretation)
{
    // Create temporary files for KNN test
    const std::string KNN_NODES_FILE = "knn_test_nodes.txt";
    const std::string KNN_NODES_FILE_INPUT = "1\t1.0,NAN\t0\n2\t2.0,3.0\t1\n3\t3.0,4.0\t0\n4\tNAN,5.0\t1";
    const std::string KNN_EDGE_FILE = "knn_test_edges.txt";
    const std::string KNN_EDGE_FILE_INPUT = "1 2\n2 3\n3 4\n1 4";

    createTempFile(KNN_NODES_FILE, KNN_NODES_FILE_INPUT);
    createTempFile(KNN_EDGE_FILE, KNN_EDGE_FILE_INPUT);

    // Initialize Graph for KNN
    Graph knnGraph(KNN_NODES_FILE, KNN_EDGE_FILE);

    // Run KNN to interpret missing features
    KNN knn(std::make_shared<Graph>(knnGraph));
    knn.configure({{"k", 3}, {"maxIterations", 5}});
    knn.run();

    // Save interpreted features
    const std::string SAVED_FEATURES_FILE = "saved_features.txt";
    knnGraph.saveFeatures(SAVED_FEATURES_FILE);

    // Read back the saved features and verify
    std::ifstream inFile(SAVED_FEATURES_FILE);
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(inFile, line))
    {
        lines.push_back(line);
    }
    inFile.close();

    // Expected lines after KNN interpretation
    std::vector<std::string> expectedLines = {
        "1\t1.0,2.0\t0",
        "2\t2.0,3.0\t1",
        "3\t3.0,4.0\t0",
        "4\t3.0,5.0\t1" // Assuming KNN filled the missing value with 3.0
    };

    EXPECT_EQ(lines, expectedLines);

    // Cleanup
    std::remove(KNN_NODES_FILE.c_str());
    std::remove(KNN_EDGE_FILE.c_str());
    std::remove(SAVED_FEATURES_FILE.c_str());
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
