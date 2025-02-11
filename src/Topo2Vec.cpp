#include <random>	
#include "Topo2Vec.hpp"



std::vector<std::vector<double>> Topo2Vec::getSample(std::vector<std::vector<double>> setOfVectors, int sampleSize)
{
    std::vector<std::vector<double>> sample;
    if (setOfVectors.empty() || sampleSize <= 0){ //Out-of-bounds check
        return sample;
    }
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, setOfVectors.size() - 1);

    for (int i = 0; i < sampleSize; i++)
    {
        sample.push_back(setOfVectors[dis(gen)]);
    }

    return sample;
}

// Stub implementations for pure virtual functions
void Topo2Vec::run() {
    // Placeholder implementation
}

std::shared_ptr<Graph<double>> Topo2Vec::extractResults() const {
    // Providing dummy file paths (adjust as needed)
    return std::make_shared<Graph<double>>("dummy_nodes.txt", "dummy_edges.txt");
}

void Topo2Vec::configure(const std::map<std::string, double> &params) {
    // Placeholder implementation
}

void Topo2Vec::reset() {
    // Placeholder implementation
}