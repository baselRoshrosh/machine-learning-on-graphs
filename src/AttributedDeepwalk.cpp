#include "AttributedDeepwalk.hpp"

#include <random>

void AttributedDeepwalk::run() {
    // TODO: Implement or leave empty if not used
}

std::shared_ptr<Graph> AttributedDeepwalk::extractResults() const {
    // TODO: Implement or leave empty if not used
    return graph;
}

void AttributedDeepwalk::configure(const std::map<std::string, double>& params) {
    // TODO: Implement or leave empty if not used
}

void AttributedDeepwalk::reset() {
    // TODO: Implement or leave empty if not used
}




std::vector<int> AttributedDeepwalk::randomWalk(int startNodeID) {
    std::vector<int> walk;
    walk.push_back(startNodeID);

    std::random_device rd;
    std::mt19937 gen(rd()); // Random number generator

    for (int i = 0; i < walkLength-1; ++i) {
        int current = walk.back();
        std::vector<int> neighbors = graph->getNeighbors(current);

        if (neighbors.empty()) break; // Stop if no neighbors

        // Select next node randomly (uniform sampling for now)
        std::uniform_int_distribution<> dist(0, neighbors.size() - 1);
        int nextNode = neighbors[dist(gen)];

        walk.push_back(nextNode);
    }
    
    return walk;
}
