#include <gtest/gtest.h>
#include "Topo2Vec.hpp"

// Public Wrapper Class for Testing
class TestableTopo2Vec : public Topo2Vec {
public:
    using Topo2Vec::getSample;  // Expose the private method
};


// Test 1: Basic Sampling Test
TEST(Topo2VecTest, BasicSampling) {
    TestableTopo2Vec topo;
    std::vector<std::vector<double>> vectors = {
        {1.0, 2.0}, {3.0, 4.0}, {5.0, 6.0}, {7.0, 8.0}
    };

    int sampleSize = 2;
    auto sample = topo.getSample(vectors, sampleSize);

    EXPECT_EQ(sample.size(), sampleSize);  // Ensure correct sample size
    for (const auto& vec : sample) {
        EXPECT_TRUE(std::find(vectors.begin(), vectors.end(), vec) != vectors.end());
    }
}

// Test 2: Empty Input Test
TEST(Topo2VecTest, EmptyInput) {
    TestableTopo2Vec topo;
    std::vector<std::vector<double>> vectors;

    auto sample = topo.getSample(vectors, 3);
    EXPECT_TRUE(sample.empty());
}

// Test 3: Sample Size Larger Than Set
TEST(Topo2VecTest, SampleSizeLargerThanSet) {
    TestableTopo2Vec topo;
    std::vector<std::vector<double>> vectors = {
        {1.0, 2.0}, {3.0, 4.0}
    };

    int sampleSize = 5;  // Requesting more samples than available vectors
    auto sample = topo.getSample(vectors, sampleSize);

    EXPECT_EQ(sample.size(), sampleSize);  // Function allows duplicates
    for (const auto& vec : sample) {
        EXPECT_TRUE(std::find(vectors.begin(), vectors.end(), vec) != vectors.end());
    }
}

// Test 4: Zero and Negative Sample Size Test
TEST(Topo2VecTest, ZeroOrNegativeSampleSize) {
    TestableTopo2Vec topo;
    std::vector<std::vector<double>> vectors = {
        {1.0, 2.0}, {3.0, 4.0}
    };

    auto sampleZero = topo.getSample(vectors, 0);
    auto sampleNegative = topo.getSample(vectors, -3);

    EXPECT_TRUE(sampleZero.empty());
    EXPECT_TRUE(sampleNegative.empty());
}
