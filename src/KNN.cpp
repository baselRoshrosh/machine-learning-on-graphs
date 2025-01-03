#include <iostream>
#include <vector>


#include "Graph.tpp" 
#include "Node.tpp"

// Distance of Nodes a and b euklid?
float Distance(const std::vector<float>& a, const std::vector<float>& b){
    float distance = 0.0;
    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i] != "#" && b[i] != "#") { // 
            distance += std::pow(a[i] - b[i], 2);
        }
    }
    return std::sqrt(distance);

}


void estimateFeatures(Graph& graph, int nodeId, int k) {
    Node& node = graph.getNode(nodeId); 
    std::vector<float> nodeFeatures = node.getFeatureVector(); 
    std::vector<int> neighbors = graph.getNeighbors(nodeId); 

    std::vector<std::pair<float, int>> distances; 

    // Distance to all Neighbors
    for (int neighborId : neighbors) {
        Node& neighbor = graph.getNode(neighborId);
        std::vector<float> neighborFeatures = neighbor.getFeatures();
        
        
        
        float distance = Distance(nodeFeatures, neighborFeatures);  //Distance function needed
        
    }

    //ggf sort for distances


    //choose k neighbors



int main():{


    //Graph graph("/input/twitch_features.txt","/input/twitch_edges.txt");


}