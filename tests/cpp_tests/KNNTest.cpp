#include <gtest/gtest.h>
#include <fstream>
#include <cstdio>
#include <memory>

#include "Graph.hpp"
#include "KNN.hpp"

using namespace std;

const string NODES_FILE = "../input/cornell/cornell_mcar_0.5.txt";
const string EDGE_FILE = "../input/cornell/cornell_edges.txt";

// Fixture class for KNN testing
class KNNTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Initialize Graph using test files
        graph = make_shared<Graph>(NODES_FILE, EDGE_FILE);
    }

    void TearDown() override
    {
        graph.reset();
    }

    shared_ptr<Graph> graph;
};

// Test if KNN can estimate missing features
TEST_F(KNNTest, EstimateFeaturesFillsMissingValuesThroughRun)
{
    KNN knn(graph);
    knn.configure({{"k", 3}, {"maxIterations", 5}});

    // Check if node 1 had missing values before running KNN
    vector<double> beforeRun = graph->getFeatureById(1);
    bool hadMissingValues = false;
    for (double feature : beforeRun)
    {
        if (isnan(feature))
        {
            hadMissingValues = true;
            break;
        }
    }
    EXPECT_TRUE(hadMissingValues); // Ensure at least one NaN existed before

    // Run KNN and check again
    knn.run();
    vector<double> afterRun = graph->getFeatureById(1);

    for (double feature : afterRun)
    {
        EXPECT_FALSE(isnan(feature)); // Ensure all missing values are filled
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
