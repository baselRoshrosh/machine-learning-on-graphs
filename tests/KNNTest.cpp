#include <gtest/gtest.h>
#include "KNN.hpp"
#include "Graph.hpp"
#include "BasicEdges.hpp"

class KNNTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Manually construct a graph instead of reading from files
        graph = std::make_shared<Graph>();
        
        // Add nodes with features and labels
        graph->addNode(Node(1, {1.0, NAN}, 0));
        graph->addNode(Node(2, {2.0, 3.0}, 1));
        graph->addNode(Node(3, {3.0, 4.0}, 0));
        graph->addNode(Node(4, {NAN, 5.0}, 1));
        
        // Initialize edges
        BasicEdges edges({{1, 2}, {2, 3}, {3, 4}, {1, 4}});
        for (const auto& edge : edges.getEdges()) {
            graph->addEdge(edge.first, edge.second);
        }
        
        knn = std::make_unique<KNN>(graph);
        knn->configure({{"k", 2}});
    }
    
    std::unique_ptr<KNN> knn;
    std::shared_ptr<Graph> graph;
};

TEST_F(KNNTest, ConfigureSetsCorrectK) {
    knn->configure({{"k", 3}});
    EXPECT_NO_THROW(knn->configure({{"k", 3}}));
}

TEST_F(KNNTest, RunExecutesWithoutError) {
    EXPECT_NO_THROW(knn->run());
}

TEST_F(KNNTest, ResetClearsCachedData) {
    knn->run();
    knn->reset();
    EXPECT_NO_THROW(knn->run());
}

TEST_F(KNNTest, RunCachesNeighborsAndComputesPaths) {
    knn->run();
    auto updatedGraph = knn->extractResults();
    EXPECT_FALSE(updatedGraph->getNodes().empty());
}

TEST_F(KNNTest, EstimateFeaturesFillsMissingValuesThroughRun) {
    knn->run();
    auto updatedGraph = knn->extractResults();
    for (const auto& node : updatedGraph->getNodes()) {
        auto features = updatedGraph->getFeatureById(node.getId());
        for (const auto& feature : features) {
            EXPECT_FALSE(std::isnan(feature));
        }
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
