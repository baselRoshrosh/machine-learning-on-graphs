#include <gtest/gtest.h>
#include "Node.hpp"

TEST(NodeTest, Constructor) {
    Node node(1, {0.5, 1.5, 2.5}, 42);
    EXPECT_EQ(node.getId(), 1);
    EXPECT_EQ(node.getFeatureVector(), std::vector<double>({0.5, 1.5, 2.5}));
    EXPECT_EQ(node.getLabel(), 42);
}

TEST(NodeTest, Getters) {
    Node node(2, {1.0, 2.0, 3.0}, 7);
    EXPECT_EQ(node.getId(), 2);
    EXPECT_EQ(node.getFeatureVector(), std::vector<double>({1.0, 2.0, 3.0}));
    EXPECT_EQ(node.getLabel(), 7);
}

TEST(NodeTest, DefaultConstructor) {
    Node node;
    EXPECT_TRUE(node.getFeatureVector().empty());
}

TEST(NodeTest, SetFeatureVector) {
    Node node(3, {2.0, 4.0, 6.0}, 9);

    std::vector<double> newFeatures = {3.3, 6.6, 9.9};
    node.setFeatureVector(newFeatures);

    EXPECT_EQ(node.getFeatureVector(), newFeatures);
}
