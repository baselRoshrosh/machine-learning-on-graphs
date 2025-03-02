#include "Topo2Vec.hpp"

#include <algorithm>
#include <iterator>
#include <random>
#include <queue>
#include <cmath>

/*
 * ======= Declaration of local helper functions ===================
 */

double getCandidateParticipation(std::shared_ptr<Graph>, const std::vector<int> &, int);
void initializeEmbeddings(std::shared_ptr<Graph>, std::unordered_map<int, std::vector<double>> &, int);
void l2normalize(std::unordered_map<int, std::vector<double>> &);
int getAverageDegree(std::shared_ptr<Graph>);
void filterAndSort(std::vector<int> &, std::vector<double> &, double);
double dotProduct(const std::vector<double> &, const std::vector<double> &);
double sigmoid(double);

/*
 * ======= Implementation of IStrategy Interface methods =============
 */
void Topo2Vec::run() {}
std::shared_ptr<Graph> Topo2Vec::extractResults() const
{
    return graph;
}
void Topo2Vec::configure(const std::map<std::string, double> &params) {}
void Topo2Vec::reset() {}


/*
 * ======= createEmbeddings() with helper functions ======================
 */

std::unordered_map<int, std::vector<double>> Topo2Vec::createEmbeddings(int dimensions)
{
    // 1: initialize random embeddings
    std::unordered_map<int, std::vector<double>> embeddings;
    initializeEmbeddings(graph, embeddings, dimensions);

    // 2: create a context subgraph for each node
    std::vector<std::vector<int>> contextSubgraphs = getContextSubgraphs();

    // 3: optimize embeddings based on subgraphs
    skipGram(embeddings, contextSubgraphs);
    l2normalize(embeddings);

    return embeddings;
}

std::vector<std::vector<int>> Topo2Vec::getContextSubgraphs()
{
    std::vector<std::vector<int>> contextSubgraphs;

    std::vector<int> nodeIDs = graph->getNodes();
    int nodeCount = graph->getNodeCount();
    int avgDegree = getAverageDegree(graph);

    std::unordered_map<int, bool> visited(nodeCount);
    std::vector<int> templist; // named as in the paper

    // used for calculating denominator of SA score
    std::unordered_set<int> subgraphNodes; // enables node lookup in O(1)
    int edgesInSubgraphCount;

    std::vector<double> naScores;
    double naScore;

    for (int currentNodeID : nodeIDs)
    {
        // reset variables
        std::for_each(visited.begin(), visited.end(), [](auto &pair)
                      { pair.second = false; });
        visited[currentNodeID] = true;
        subgraphNodes.clear();
        edgesInSubgraphCount = 0;

        templist = graph->getNeighbors(currentNodeID);
        if (templist.size() == 0)
        {
            continue;
        }

        for (int candidateNodeID : templist)
        {
            naScore = getCandidateParticipation(graph, templist, candidateNodeID) / graph->getNeighbors(candidateNodeID).size();
            naScores.emplace_back(naScore);
        }
        filterAndSort(templist, naScores, tau); // templist is now the context subgraph for the current Node

        // count edges in subgraph
        for (int nodeID : templist)
        {
            // Count edges with already present nodes
            for (int neighbor : graph->getNeighbors(nodeID))
            {
                if (subgraphNodes.count(neighbor)) // If edge already in subgraph, count it
                {
                    edgesInSubgraphCount++;
                }
            }

            subgraphNodes.insert(nodeID); // Add node to subgraph
        }

        // expand subgraph k times
        for (int k = 0; k < avgDegree; k++)
        {
            expandSubgraph(templist, visited, subgraphNodes, edgesInSubgraphCount);
        }

        contextSubgraphs.push_back(templist);
    }

    return contextSubgraphs;
}

void Topo2Vec::expandSubgraph(std::vector<int> &templist, std::unordered_map<int, bool> &visited, std::unordered_set<int> &subgraphNodes, int &edgesInSubgraphCount)
{
    // creating a copy of the templist with only unique nodeIDs
    std::vector<int> returnTemplist(templist);
    std::vector<int>::iterator it;
    it = std::unique(returnTemplist.begin(), returnTemplist.end());
    returnTemplist.resize(std::distance(returnTemplist.begin(), it));

    std::vector<int> nodeIDs = graph->getNodes();

    std::vector<int> candidateNodeNeighbors;
    double naScore, saScore;
    std::vector<double> naScores, saScores;

    for (int candidateNodeID : templist)
    {
        if (visited[candidateNodeID])
        {
            continue;
        }

        visited[candidateNodeID] = true;
        candidateNodeNeighbors = graph->getNeighbors(candidateNodeID);

        // expand with neighborhood-important neighbors
        for (int candidateNodeNeighborID : candidateNodeNeighbors)
        {
            naScore = getCandidateParticipation(graph, templist, candidateNodeNeighborID) / graph->getNeighbors(candidateNodeNeighborID).size();
            naScores.emplace_back(naScore);
        }
        filterAndSort(candidateNodeNeighbors, naScores, tau);

        // expand with subgraph-important neighbors
        for (int candidateNodeNeighborID : candidateNodeNeighbors)
        {
            saScore = getCandidateParticipation(graph, templist, candidateNodeNeighborID) / edgesInSubgraphCount;
            saScores.emplace_back(saScore);
        }
        filterAndSort(candidateNodeNeighbors, saScores, tau);

        // count added edges in subgraph
        for (int candidateNodeID : candidateNodeNeighbors)
        {
            for (int neighborID : graph->getNeighbors(candidateNodeID))
            {
                if (subgraphNodes.count(neighborID))
                {
                    edgesInSubgraphCount++;
                }
            }

            subgraphNodes.insert(candidateNodeID);
        }

        returnTemplist.insert(returnTemplist.end(), candidateNodeNeighbors.begin(), candidateNodeNeighbors.end());
    }

    templist.insert(templist.end(), returnTemplist.begin(), returnTemplist.end());
}


/**
 * calculates how many of the neighbors of a candidate node are already in the subgraph
 *
 * @param graph the complete graph to get Neighbors of the candidate Node
 * @param templist the current subgraph
 * @param candidateNodeID ID of the node we might add to the subgraph
 *
 * @return how many neighbors of the candidateNode are in the subgraph
 */
double getCandidateParticipation(std::shared_ptr<Graph> graph, const std::vector<int> &templist, int candidateNodeID)
{
    std::vector<int> subgraph(templist);
    std::vector<int> connectedNodes = graph->getNeighbors(candidateNodeID);

    // calculate intersection of the neighbors of the candidate Node and the subgraph
    std::vector<int> intersectionSubgraphConnectedNodes;
    std::sort(subgraph.begin(), subgraph.end());
    std::sort(connectedNodes.begin(), connectedNodes.end());
    std::set_intersection(connectedNodes.begin(), connectedNodes.end(), subgraph.begin(), subgraph.end(), std::back_inserter(intersectionSubgraphConnectedNodes));

    return intersectionSubgraphConnectedNodes.size(); // size of intersections equals candidate participation
}

/**
 * creates a randomized embedding of the given dimension for each node
 *
 * @param graph[in] the original graph
 * @param embeddings[in, out] pointer to the embeddings<nodeID, embedding>
 * @param dimensions[in] how many dimensions an embedding should have
 */
void initializeEmbeddings(std::shared_ptr<Graph> graph, std::unordered_map<int, std::vector<double>> &embeddings, int dimensions)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(-0.5 / dimensions, 0.5 / dimensions);

    for (int node : graph->getNodes())
    {
        std::vector<double> vec(dimensions);
        for (double &val : vec)
        {
            val = dist(gen);
        }
        embeddings[node] = vec;
    }
}



/**
 * performs l2 normalization in place on the vectors inside an int-vector map.
 *
 * @param embeddings[in, out] a map <int, vector<double>> containing embeddings for nodes
 */
void l2normalize(std::unordered_map<int, std::vector<double>> &embeddings)
{
    for (auto &[nodeID, embedding] : embeddings)
    {
        double norm = 0.0;

        // caculating norm factor
        for (double value : embedding)
        {
            norm += std::pow(value, 2);
        }
        norm = std::sqrt(norm);

        // normalize
        if (norm > 0)
        {
            for (double &value : embedding)
            {
                value /= norm;
            }
        }
    }
}

/**
 * ====== genereal helper methods ===========================
 */

/**
 * calculates avg. degree of graph
 *
 * @param graph the graph to calculate the avg. degree of
 * @return the avg. degree
 */
int getAverageDegree(std::shared_ptr<Graph> graph)
{
    return graph->getEdgeCount() / graph->getNodeCount();
}

/**
 * Helper-function to sort a vector of IDs by respective scores if they pass a given threshhold.
 *
 * @param list[in, out]
 * @param scores[in]
 * @param threshhold[in]
 * @param edgesCounter[in, out]
 */
void filterAndSort(std::vector<int> &list, std::vector<double> &scores, double threshhold)
{
    std::vector<std::pair<int, double>> idScorePairs;

    for (size_t i = 0; i < list.size(); ++i)
    {
        if (scores[i] >= threshhold)
        {
            idScorePairs.emplace_back(list[i], scores[i]);
        }
    }

    // Sort pairs based on score in descending order
    std::sort(idScorePairs.begin(), idScorePairs.end(),
              [](const std::pair<int, double> &a, const std::pair<int, double> &b)
              {
                  return a.second > b.second;
              });

    // Extract sorted IDs
    list.clear();
    for (const auto &pair : idScorePairs)
    {
        list.push_back(pair.first);
    }
}

/**
 * calculates dot product of two vectors. Their order is ambigous
 *
 * @param vec1 first vector of the equation
 * @param vec2 second vector of the equation
 *
 * @return the calculated dot product
 */
double dotProduct(const std::vector<double> &vec1, const std::vector<double> &vec2)
{
    double result = 0.0;
    for (size_t i = 0; i < vec1.size(); i++)
    {
        result += vec1[i] * vec2[i];
    }
    return result;
}

/**
 * calculates sigmoid value for a variable
 *
 * @param x the specified variable
 *
 * @return its sigmoid value
 */
double sigmoid(double x)
{
    return 1.0 / (1.0 + std::exp(-x));
}
