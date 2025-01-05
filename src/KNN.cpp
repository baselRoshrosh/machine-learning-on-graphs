#include <iostream>
#include <vector>
#include <cmath>


#include "Graph.tpp" 
#include "Node.tpp"

// Distance of Nodes a and b   using l2 distance
float l2Distance(const std::vector<double>& a, const std::vector<double>& b){
    float distance = 0.0;
    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i] != "#" && b[i] != "#") { // 
            distance += std::pow(a[i] - b[i], 2);
        }
    }
    return std::sqrt(distance);    //might not need the sqrt to compare distances and save some computing time

}



void estimateFeatures(Graph& graph, int nodeId, int k) {
    Node& node = graph.getNode(nodeId); 
    std::vector<float> nodeFeatures = node.getFeatureVector(); 
    std::vector<int> neighbors = graph.getNeighbors(nodeId); 



    std::vector<std::pair<float, int>> distances; 



//distances to each neighbor
    for (int neighborId : neighbors) {
        Node& neighbor = graph.getNode(neighborId);
        std::vector<float> neighborFeatures = neighbor.getFeatures();
        float distance = l2Distance(nodeFeatures, neighborFeatures);
        distances.push_back({distance, neighborId});
    }



    //choose k neigbors
    std::vector<int> knn;
    for (int i = 0; i < std::min<int>(k, distances.size()); ++i) {
        knn.push_back(distances[i].second);
    }

   //estimating features that are missing
    for (int i = 0; i < nodeFeatures.size(); ++i) {
        if (nodeFeatures[i] == "#") { 
            float sum = 0.0;
            int count = 0;


            for (int neighborId : knn) {
                
            }

        }
    }

}

int main():{


    //Graph graph("/input/twitch_features.txt","/input/twitch_edges.txt");





}