#ifndef KNN_HPP
#define KNN_HPP

#include <iostream>
#include <cmath>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <algorithm>
#include <limits>
#include <iomanip>
#include <fstream>
#include "../src/Graph.tpp"
#include "../src/BasicEdges.cpp"




template <typename T>
class KNN {
private:
    //Cache for neighbors and path to avoid repeatedly calculating them
    std::unordered_map<int, std::vector<int>> cachedNeighbors;          
    std::unordered_map<int, std::unordered_map<int, int>> precomputedPaths; 

    void cacheNeighbors(const Graph<T>& graph);

    void calcPaths(const Graph<T>& graph);

public:

    void estimateFeatures(Graph<T>& graph, int k);
};


#include "../src/KNN.tpp"
#endif // KNN_HPP
