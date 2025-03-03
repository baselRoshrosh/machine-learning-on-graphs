#include "AttributedDeepwalk.hpp"

#include <set>
#include <algorithm>
#include <unordered_set>
#include <queue>
#include <numeric>
#include <cmath>

/*
 * ======= declaration of local functions ======
 */
std::set<int> calculateCover(int, int, std::shared_ptr<Graph>);

/*
 * ======= implementation of strategy methods ========== 
 */
void AttributedDeepwalk::run() {
    std::vector<std::vector<int>> rawEmbeddings = csadw();  // csadw() returns vector<vector<int>>

    // Convert vector<vector<int>> to unordered_map<int, vector<double>>
    std::unordered_map<int, std::vector<double>> embeddings;
    for (size_t i = 0; i < rawEmbeddings.size(); ++i) {
        embeddings[static_cast<int>(i)] = std::vector<double>(rawEmbeddings[i].begin(), rawEmbeddings[i].end());
    }

    for (const auto &node : graph->getNodes()) {
        std::unordered_map<int, std::vector<double>> sample = getSample(embeddings, coverDepth); 

        if (sample.empty()) continue;  // Ensuring there is at least one valid sample

        std::vector<std::vector<double>> nodeList = getSimilarNodes(embeddings, sample.begin()->second, coverDepth); 

        //TODO
        //guessFeatures(node, nodeList);
    }
}


std::shared_ptr<Graph> AttributedDeepwalk::extractResults() const
{
    return graph;
}
void AttributedDeepwalk::configure(const std::map<std::string, double>& params) {
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
}
void AttributedDeepwalk::reset() {
    fusionCoefficient = 0.5;
    coverDepth = 2;
    walkLength = 80;
    walksPerNode = 10;
}


std::vector<std::vector<int>> AttributedDeepwalk::csadw() {
    // TODO:  placeholder
    return std::vector<std::vector<int>>();
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

std::unordered_map<int, std::vector<std::pair<double, size_t>>> AttributedDeepwalk::getAliasTables() const
{
    std::unordered_map<int, std::vector<std::pair<double, size_t>>> aliasTables;

    // create alias table for each node
    for (int node : graph->getNodes())
    {
        // 1: get weights of edges to neighbors
        std::vector<double> neighborWeights;
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

        std::vector<double> neighborProbabilites(neighborWeights.size()); // new vector for clarity, initialized to 0.0s
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
        std::vector<std::pair<double, size_t>> aliasTable(numNeighbors, {0.0, std::numeric_limits<size_t>::max()});
        std::queue<size_t>
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

    return aliasTables;
}


std::vector<int> AttributedDeepwalk::randomWalk(int startNodeID) {
    std::vector<int> walk;     
    //TODO This is just a placeholder
    return walk;
}

double AttributedDeepwalk::measuring_attribute_similarity(int node1, int node2) const
{
    std::vector<double> featuresNode1 = graph->getFeatureById(node1);
    std::vector<double> featuresNode2 = graph->getFeatureById(node2);

    // calculating cosine-similarity between feature vectors
    // Compute dot product
    double dotProduct = std::inner_product(featuresNode1.begin(), featuresNode1.end(), featuresNode2.begin(), 0.0);

    // Compute norms
    double norm1 = std::sqrt(std::inner_product(featuresNode1.begin(), featuresNode1.end(), featuresNode1.begin(), 0.0));
    double norm2 = std::sqrt(std::inner_product(featuresNode2.begin(), featuresNode2.end(), featuresNode2.begin(), 0.0));

    // Handle division by zero
    if (norm1 == 0.0 || norm2 == 0.0)
    {
        return 0.0; // Convention: return 0 if any vector is zero
    }

    double similarity = dotProduct / (norm1 * norm2);
    return std::max(similarity, 0.0);
}

double AttributedDeepwalk::measuring_structural_similarity(int node1, int node2) const
{
    std::set<int> coverNode1 = calculateCover(node1, coverDepth, graph);
    std::set<int> coverNode2 = calculateCover(node2, coverDepth, graph);

    // calculate terms
    std::set<int> intersectionOfCovers;
    std::set<int> unionOfCovers;

    std::set_intersection(coverNode1.begin(), coverNode1.end(), coverNode2.begin(), coverNode2.end(), std::inserter(intersectionOfCovers, intersectionOfCovers.begin()));
    std::set_union(coverNode1.begin(), coverNode1.end(), coverNode2.begin(), coverNode2.end(), std::inserter(unionOfCovers, unionOfCovers.begin()));

    if (unionOfCovers.size() == 0) // shouldn't technically occur because cover includes node itself
    {
        return 0.0;
    }

    return intersectionOfCovers.size() / unionOfCovers.size();
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
std::set<int> calculateCover(int node, int coverDepth, std::shared_ptr<Graph> graph)
{
    std::set<int> cover;
    std::queue<std::pair<int, int>> q;

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
int sampleFromAliasTable(const std::vector<std::pair<double, size_t>>& aliasTable, std::mt19937& gen) {
    std::uniform_int_distribution<> int_dis(0, aliasTable.size() - 1);
    std::uniform_real_distribution<> real_dis(0.0, 1.0);
    
    size_t column = int_dis(gen);
    double coinToss = real_dis(gen);
    
    if (coinToss < aliasTable[column].first)
        return column;
    else
        return aliasTable[column].second;
}

std::vector<int> AttributedDeepwalk::randomWalk(int startNodeID) {
    std::vector<int> walk;
    walk.push_back(startNodeID);

    // Compute alias tables on the fly (once per random walk)
    const auto aliasTables = getAliasTables();

    std::random_device rd;
    std::mt19937 gen(rd());

    // Assuming aliasTables is a member variable of type:
    // std::unordered_map<int, std::vector<std::pair<double, size_t>>>
    // that you precomputed with getAliasTables()
    for (int i = 0; i < walkLength - 1; ++i) {
        int current = walk.back();
        std::vector<int> neighbors = graph->getNeighbors(current);
        if (neighbors.empty())
            break;

        // Get the alias table corresponding to the current node
        const auto& aliasTable = aliasTables.at(current);
        
        // Sample an index from the alias table (each index corresponds to a neighbor)
        int neighborIdx = sampleFromAliasTable(aliasTable, gen);
        int nextNode = neighbors[neighborIdx];

        walk.push_back(nextNode);
    }
    
    return walk;
}

std::unordered_map<int, std::vector<double>> AttributedDeepwalk::csadw()
{
    calculateWeightMatrix();

    std::vector<std::vector<int>> randomWalks;
    std::vector<int> nodes = graph->getNodes();
    std::mt19937 gen(std::random_device{}());

    for (int iter = 0; iter < walksPerNode; ++iter)
    {
        std::shuffle(nodes.begin(), nodes.end(), gen);
        for (int node : nodes)
        {
            randomWalks.push_back(randomWalk(node));
        }
    }

    std::unordered_map<int, std::vector<double>> embeddings;
    for (int node : graph->getNodes())
    {
        embeddings[node] = std::vector<double>(embeddingDimensions, 0.01);
    }

    skipGram(embeddings, randomWalks);
    return embeddings;
}

