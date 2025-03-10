#include "AttributedDeepwalk.hpp"

#include <set>
#include <algorithm>
#include <unordered_set>
#include <queue>
#include <numeric>
#include <cmath>
#include <stdexcept>
#include <iostream>

using namespace std;

/*
 * ======= declaration of local functions ======
 */
set<int> calculateCover(int, int, shared_ptr<Graph>);

/*
 * ======= implementation of strategy methods ========== 
 */
void AttributedDeepwalk::run() {
    unordered_map<int, vector<double>> rawEmbeddings = csadw();

    // Convert vector<vector<int>> to unordered_map<int, vector<double>>
    unordered_map<int, vector<double>> embeddings;
    for (size_t i = 0; i < rawEmbeddings.size(); ++i) {
        embeddings[static_cast<int>(i)] = vector<double>(rawEmbeddings[i].begin(), rawEmbeddings[i].end());
    }

    for (const auto &node : graph->getNodes()) {
        unordered_map<int, vector<double>> sample = getSample(embeddings, coverDepth); 

        if (sample.empty()) continue;  // Ensuring there is at least one valid sample

        vector<vector<double>> nodeList = getFeaturesOfSimilarNodes(embeddings, sample.begin()->second, coverDepth); 


        guessFeatures(node, nodeList);
    }
}


shared_ptr<Graph> AttributedDeepwalk::extractResults() const
{
    return graph;
}

void AttributedDeepwalk::configure(const map<string, double>& params) {
    if (params.find("fusionCoefficient") != params.end()) {
        fusionCoefficient = params.at("fusionCoefficient");
    }
    if (params.find("coverDepth") != params.end()) {
        coverDepth = static_cast<int>(params.at("coverDepth"));
    }
    if (params.find("walkLength") != params.end()) {
        walkLength = static_cast<int>(params.at("walkLength"));
    }
    if (params.find("walksPerNode") != params.end()) {
        walksPerNode = static_cast<int>(params.at("walksPerNode"));
    }
    if (params.find("embeddingDimensions") != params.end())
    {
        embeddingDimensions = static_cast<int>(params.at("embeddingDimensions"));
    }
    if (params.find("numEpochs") != params.end())
    {
        numEpochs = static_cast<int>(params.at("numEpochs"));
    }
    if (params.find("sampleSize") != params.end())
    {
        sampleSize = static_cast<int>(params.at("sampleSize"));
    }
    if (params.find("k") != params.end())
    {
        k = static_cast<int>(params.at("k"));
    }
    if (params.find("windowSize") != params.end())
    {
        windowSize = static_cast<int>(params.at("windowSize"));
    }
    if (params.find("numNegativeSamples") != params.end())
    {
        numNegativeSamples = static_cast<int>(params.at("numNegativeSamples"));
    }
    if (params.find("learningRate") != params.end())
    {
        learningRate = params.at("learningRate");
    }
}

void AttributedDeepwalk::reset() {
    fusionCoefficient = 0.5;
    coverDepth = 2;
    walkLength = 80;
    walksPerNode = 10;
}

/*
 * ======= calculating Alias Tables ============
 */
void AttributedDeepwalk::calculateWeightMatrix()
{
    double weight;

    for (auto [source, destination] : graph->getEdges())
    {
        weight = fusionCoefficient * measuring_attribute_similarity(source, destination) + (1 - fusionCoefficient) * measuring_structural_similarity(source, destination);
        graph->setEdgeWeight(source, destination, weight);
    }
}

void AttributedDeepwalk::computeAliasTables()
{
    aliasTables.clear(); //// Clear previous alias tables to avoid memory issues

    // create alias table for each node
    for (int node : graph->getNodes())
    {
        // 1: get weights of edges to neighbors
        vector<double> neighborWeights;
        for (int neighbor : graph->getNeighbors(node))
        {
            neighborWeights.push_back(graph->getEdgeWeight(node, neighbor));
        }

        // 2: normalize to get transition probabilites resembling the edge weight
        double weightSum = 0;
        for (double weight : neighborWeights)
        {
            weightSum += weight;
        }

        vector<double> neighborProbabilites(neighborWeights.size()); // new vector for clarity, initialized to 0.0s
        if (weightSum != 0)
        {
            for (int i = 0; i < neighborWeights.size(); i++)
            {
                neighborProbabilites[i] = neighborWeights[i] / weightSum;
            }
        }

        /*
         * 3: calculate alias table
         *
         * following with slight modifications and added comments:
         * https://gist.github.com/Liam0205/0b5786e9bfc73e75eb8180b5400cd1f8
         */
        const size_t numNeighbors = neighborProbabilites.size();
        vector<pair<double, size_t>> aliasTable(numNeighbors, {0.0, numeric_limits<size_t>::max()});
        queue<size_t>
            underfull, // where probability table < 1
            overfull;  // where probability table > 1

        // initializing the alias table with U_i = n*p_i (see Wikipedia)
        for (size_t i = 0; i != numNeighbors; ++i)
        {
            aliasTable[i].first = numNeighbors * neighborProbabilites[i];
            if (aliasTable[i].first < 1.0)
            {
                underfull.push(i);
            }
            else
            {
                overfull.push(i);
            }
        }

        // continue while table entries aren't exactly full
        while ((!underfull.empty()) && (!overfull.empty()))
        {
            // 1: choose overfull and underfull entry
            auto underfullEntry = underfull.front(),
                 overfullEntry = overfull.front();
            underfull.pop(), overfull.pop();

            // 2: unused space in underfullEntry becomes outcome of overfullEntry
            aliasTable[underfullEntry].second = overfullEntry;

            // 3: removing allocated space from overfullEntry
            aliasTable[overfullEntry].first -= (1.0 - aliasTable[underfullEntry].first);

            if (aliasTable[overfullEntry].first < 1.0)
            {
                underfull.push(overfullEntry);
            }
            else
            {
                overfull.push(overfullEntry);
            }
        }

        // Finalize any remaining entries
        while (!underfull.empty())
        {
            size_t idx = underfull.front();
            underfull.pop();
            aliasTable[idx].first = 1.0;
            aliasTable[idx].second = idx; // Ensure valid index
        }
        while (!overfull.empty()) {
            size_t idx = overfull.front();
            overfull.pop();
            aliasTable[idx].first = 1.0;
            aliasTable[idx].second = idx; // Ensure valid index
        }
        

        aliasTables[node] = aliasTable;
    }

    for (int node : graph->getNodes()) {
        if (aliasTables.find(node) == aliasTables.end()) {
            aliasTables[node] = {};  // Ensure every node has an entry
        }
    }
}

double AttributedDeepwalk::measuring_attribute_similarity(int node1, int node2) const
{
    vector<double> featuresNode1 = graph->getFeatureById(node1);
    vector<double> featuresNode2 = graph->getFeatureById(node2);
    double dotProduct = 0;
    double norm1 = 0;
    double norm2 = 0;

    // calculating cosine-similarity between feature vectors
    // filter as they could have missing features
    for (size_t i = 0; i < featuresNode1.size(); i++)
    {
        double f1 = featuresNode1[i];
        double f2 = featuresNode2[i];

        // Ignore NaN values
        if (isnan(f1) || isnan(f2))
        {
            continue;
        }

        dotProduct += f1 * f2;
        norm1 += f1 * f1;
        norm2 += f2 * f2;
    }

    // Compute norms
    norm1 = sqrt(norm1);
    norm2 = sqrt(norm2);

    // Handle division by zero
    if (norm1 == 0.0 || norm2 == 0.0)
    {
        return 0.0; // Convention: return 0 if any vector is zero
    }

    double similarity = dotProduct / (norm1 * norm2);
    return max(similarity, 0.0);
}

double AttributedDeepwalk::measuring_structural_similarity(int node1, int node2) const
{
    set<int> coverNode1 = calculateCover(node1, coverDepth, graph);
    set<int> coverNode2 = calculateCover(node2, coverDepth, graph);

    // calculate terms
    set<int> intersectionOfCovers;
    set<int> unionOfCovers;

    set_intersection(coverNode1.begin(), coverNode1.end(), coverNode2.begin(), coverNode2.end(), inserter(intersectionOfCovers, intersectionOfCovers.begin()));
    set_union(coverNode1.begin(), coverNode1.end(), coverNode2.begin(), coverNode2.end(), inserter(unionOfCovers, unionOfCovers.begin()));

    if (unionOfCovers.size() == 0) // shouldn't technically occur because cover includes node itself
    {
        return 0.0;
    }

    return static_cast<double>(intersectionOfCovers.size()) / unionOfCovers.size();

}

/*
 * =========== local helper functions ==============
 */

/**
 * Calculates the cover of a given node by performing BFS to a specified depth
 *
 * @param node the node from which the cover should be build
 * @param coverDepth the depth to which expand the cover
 * @param graph the graph containing the node
 * @return the cover of nodes to specified depth for the given node
 */
set<int> calculateCover(int node, int coverDepth, shared_ptr<Graph> graph)
{
    set<int> cover;
    queue<pair<int, int>> q;

    cover.insert(node);
    q.push({node, 0});

    // perform BFS to specified depth
    while (!q.empty())
    {
        auto [node, depth] = q.front();
        q.pop();

        if (depth >= coverDepth)
            continue;

        for (int neighbor : graph->getNeighbors(node))
        {
            if (!cover.count(neighbor))
            {
                q.push({neighbor, depth + 1});
                cover.insert(neighbor);
            }
        }
    }

    return cover;
}

// Helper function to sample from an alias table
int sampleFromAliasTable(const vector<pair<double, size_t>>& aliasTable, mt19937& gen) {
    uniform_int_distribution<> int_dis(0, aliasTable.size() - 1);
    uniform_real_distribution<> real_dis(0.0, 1.0);
    
    size_t column = int_dis(gen);
    double coinToss = real_dis(gen);
    
    if (coinToss < aliasTable[column].first)
        return column;
    else
        return aliasTable[column].second;
}

vector<int> AttributedDeepwalk::randomWalk(int startNodeID) {
    vector<int> walk;
    walk.push_back(startNodeID);

    random_device rd;
    mt19937 gen(rd());

    for (int i = 0; i < walkLength - 1; ++i) {
        int current = walk.back();
        vector<int> neighbors = graph->getNeighbors(current);

        // Stop walk if the node has no neighbors
        if (neighbors.empty()) break;

        // Ensure alias tables exist for the node
        if (aliasTables.find(current) == aliasTables.end() || aliasTables[current].empty()) {
            break;
        }
        

        // Sample from precomputed alias table
        const auto& aliasTable = aliasTables.at(current);
        int neighborIdx = sampleFromAliasTable(aliasTable, gen);
        int nextNode = neighbors[neighborIdx];

        walk.push_back(nextNode);
    }

    return walk;
}

std::unordered_map<int, std::vector<double>> AttributedDeepwalk::csadw()
{
    calculateWeightMatrix();

    computeAliasTables();  //Compute alias tables ONCE before random walks

    vector<vector<int>> randomWalks;
    vector<int> nodes = graph->getNodes();
    mt19937 gen(random_device{}());

    for (int iter = 0; iter < walksPerNode; ++iter)
    {
        shuffle(nodes.begin(), nodes.end(), gen);
        for (int node : nodes)
        {
            randomWalks.push_back(randomWalk(node));  // Now using precomputed alias tables
        }
    }

    unordered_map<int, vector<double>> embeddings = EmbeddingStrategy::initializeEmbeddings(graph, embeddingDimensions);

    skipGram(embeddings, randomWalks);
    return embeddings;
}


