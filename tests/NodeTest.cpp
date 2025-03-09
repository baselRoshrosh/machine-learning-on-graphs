#include <gtest/gtest.h>
#include "Node.hpp"

using namespace std;

TEST(NodeTest, Constructor)
{
    Node node(91, {0.0, 0.0, 0.0}, 3);
    EXPECT_EQ(node.getId(), 91);
    EXPECT_EQ(node.getFeatureVector(), vector<double>({0.0, 0.0, 0.0}));
    EXPECT_EQ(node.getLabel(), 3);
}

TEST(NodeTest, Getters)
{
    Node node(91, {0.0, 0.0, 0.0}, 3);
    EXPECT_EQ(node.getId(), 91);
    EXPECT_EQ(node.getFeatureVector(), vector<double>({0.0, 0.0, 0.0}));
    EXPECT_EQ(node.getLabel(), 3);
}

TEST(NodeTest, DefaultConstructor)
{
    Node node;
    EXPECT_TRUE(node.getFeatureVector().empty());
}

TEST(NodeTest, SetFeatureVector)
{
    Node node(91, {0.0, 0.0, 0.0}, 3);

    vector<double> newFeatures = {1.5, 2.5, 3.5};
    node.setFeatureVector(newFeatures);

    EXPECT_EQ(node.getFeatureVector(), newFeatures);
}
