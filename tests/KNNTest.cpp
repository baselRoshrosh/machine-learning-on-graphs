#include <gtest/gtest.h>
#include <fstream>
#include <cstdio>

#include "Graph.hpp"
#include "KNN.hpp"

// Helper function to create temporary test files
void createTempFile(const std::string &filename, const std::string &content) {
    std::ofstream outFile(filename);
    outFile << content;
    outFile.close();
}

// Temporary test files
const std::string NODES_FILE = "knn_test_nodes.txt";
const std::string NODES_FILE_INPUT = "1\t1.0,NAN\t0\n2\t2.0,3.0\t1\n3\t3.0,4.0\t0\n4\tNAN,5.0\t1";
const std::string EDGE_FILE = "knn_test_edges.txt";
const std::string EDGE_FILE_INPUT = "1 2\n2 3\n3 4";  // Simple undirected edges

// Fixture class for KNN testing
class KNNTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create temporary node and edge files
        createTempFile(NODES_FILE, NODES_FILE_INPUT);
        createTempFile(EDGE_FILE, EDGE_FILE_INPUT);

        // Initialize Graph using test files
        graph = new Graph(NODES_FILE, EDGE_FILE);
    }

    void TearDown() override {
        delete graph;
        std::remove(NODES_FILE.c_str());
        std::remove(EDGE_FILE.c_str());
    }

    Graph *graph;
};

// Test if KNN can estimate missing features
TEST_F(KNNTest, EstimateFeaturesFillsMissingValuesThroughRun) {
    KNN knn;
    knn.run(*graph);  // Assuming `run()` modifies the graph and fills missing values

    // Check if missing values were estimated
    std::vector<double> features1 = graph->getFeatureById(1);
    std::vector<double> features4 = graph->getFeatureById(4);

    // Ensure missing values are replaced (adjust conditions based on KNN logic)
    for (double feature : features1) {
        ASSERT_FALSE(std::isnan(feature)); // No NaNs should remain
    }
    for (double feature : features4) {
        ASSERT_FALSE(std::isnan(feature));
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
