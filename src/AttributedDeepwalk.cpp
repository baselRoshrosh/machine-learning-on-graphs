#include "AttributedDeepwalk.hpp"

void AttributedDeepwalk::calculateWeightMatrix() {
    double weight;

    for (auto [source, destination] : graph->getEdges()) {
        weight = fusionCoefficient * measuring_attribute_similarity(source, destination) + (1 - fusionCoefficient) * measuring_structural_similarity(source, destination);
        graph->setEdgeWeight(source, destination, weight);
    }
}

std::unordered_map<int, std::vector<std::pair<double, size_t>>> getAliasTables() {
    
}

double AttributedDeepwalk::measuring_attribute_similarity(int node1, int node2) const {

}

double AttributedDeepwalk::measuring_structural_similarity(int node1, int node2) const{

}