#include <gtest/gtest.h>
#include "../include/INode.hpp"

TEST(NodeTest, Constructor) {
    Node<float> node(1, {0.5, 1.5, 2.5}, 42);
    EXPECT_EQ(node.getId(), 1);
    EXPECT_EQ(node.getFeatureVector(), std::vector<float>({0.5, 1.5, 2.5}));
    EXPECT_EQ(node.getLabel(), 42);
}

TEST(NodeTest, Getters) {
    Node<int> node(2, {1, 2, 3}, 7);
    EXPECT_EQ(node.getId(), 2);
    EXPECT_EQ(node.getFeatureVector(), std::vector<int>({1, 2, 3}));
    EXPECT_EQ(node.getLabel(), 7);
}

TEST(NodeTest, DefaultConstructor) {
    Node<double> node;
    EXPECT_TRUE(node.getFeatureVector().empty());
}
