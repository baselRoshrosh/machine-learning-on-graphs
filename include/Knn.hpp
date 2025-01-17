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

// Function to cache neighbors of all nodes in the graph
void cacheNeighbors(const Graph<double>& graph);

// Function to calculate and precompute shortest paths for all nodes in the graph
void calcPaths(const Graph<double>& graph);

// Template function to estimate features for nodes in a graph using k-nearest neighbors
template <typename T>
void estimateFeatures(Graph<T>& graph, int k);

#endif // KNN_HPP