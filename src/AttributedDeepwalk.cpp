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
    std::vector<std::vector<int>> embeddings = csadw();

    for (int node : graph->getNodes()) {

        std::vector<int> sample = getSample(embeddings, node);
        std::vector<int> nodeList = getSimilarNodes(sample, embeddings);
        // Placeholder TODO
        // guessFeatures(node, nodeList);
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