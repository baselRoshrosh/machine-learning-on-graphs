#include "Topo2Vec.hpp"

#include <algorithm>
#include <iterator>
#include <random>
#include <queue>
#include <cmath>

using namespace std;

/*
 * ======= Declaration of local helper functions ===================
 */

double getCandidateParticipation(shared_ptr<Graph>, const vector<int> &, int);
void initializeEmbeddings(shared_ptr<Graph>, unordered_map<int, vector<double>> &, int);
void l2normalize(unordered_map<int, vector<double>> &);
int getAverageDegree(shared_ptr<Graph>);
void filterAndSort(vector<int> &, vector<double> &, double);
double dotProduct(const vector<double> &, const vector<double> &);
double sigmoid(double);

/*
 * ======= Implementation of IStrategy Interface methods =============
 */
void Topo2Vec::run()
{
    auto embeddings = createEmbeddings(embeddingDimensions);

    for (auto node : graph->getNodes())
    {
        auto nodesSample = getSample(embeddings, sampleSize);
        vector<vector<double>> similarNodes = getSimilarNodes(embeddings, embeddings[node], k);
        // TODO in issue #41: Guess features based on KNN implementation 
    }
}

shared_ptr<Graph> Topo2Vec::extractResults() const
{
    return graph;
}

void Topo2Vec::configure(const map<string, double> &params)
{
    if (params.find("tau") != params.end())
    {
        tau = params.at("tau");
    }
    if (params.find("embeddingDimensions") != params.end())
    {
        embeddingDimensions = static_cast<int>(params.at("embeddingDimensions"));
    }
    if (params.find("numEpochs") != params.end())
    {
        numEpochs = static_cast<int>(params.at("numEpochs"));
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

void Topo2Vec::reset()
{
    tau = 0.5;
    embeddingDimensions = 128;
    numEpochs = 5;
    windowSize = 5;
    numNegativeSamples = 5;
    learningRate = 0.025;
}


/*
 * ======= createEmbeddings() with helper functions ======================
 */

unordered_map<int, vector<double>> Topo2Vec::createEmbeddings(int dimensions)
{
    // 1: initialize random embeddings
    unordered_map<int, vector<double>> embeddings = EmbeddingStrategy::initializeEmbeddings(graph, dimensions);

    // 2: create a context subgraph for each node
    vector<vector<int>> contextSubgraphs = getContextSubgraphs();

    // 3: optimize embeddings based on subgraphs
    skipGram(embeddings, contextSubgraphs);
    l2normalize(embeddings);

    return embeddings;
}

vector<vector<int>> Topo2Vec::getContextSubgraphs()
{
    vector<vector<int>> contextSubgraphs;

    vector<int> nodeIDs = graph->getNodes();
    int nodeCount = graph->getNodeCount();
    int avgDegree = getAverageDegree(graph);

    unordered_map<int, bool> visited(nodeCount);
    vector<int> templist; // named as in the paper

    // used for calculating denominator of SA score
    unordered_set<int> subgraphNodes; // enables node lookup in O(1)
    int edgesInSubgraphCount;

    vector<double> naScores;
    double naScore;

    for (int currentNodeID : nodeIDs)
    {
        // reset variables
        for_each(visited.begin(), visited.end(), [](auto &pair)
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

void Topo2Vec::expandSubgraph(vector<int> &templist, unordered_map<int, bool> &visited, unordered_set<int> &subgraphNodes, int &edgesInSubgraphCount)
{
    // creating a copy of the templist with only unique nodeIDs
    vector<int> returnTemplist(templist);
    vector<int>::iterator it;
    it = unique(returnTemplist.begin(), returnTemplist.end());
    returnTemplist.resize(distance(returnTemplist.begin(), it));

    vector<int> nodeIDs = graph->getNodes();

    vector<int> candidateNodeNeighbors;
    double naScore, saScore;
    vector<double> naScores, saScores;

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
double getCandidateParticipation(shared_ptr<Graph> graph, const vector<int> &templist, int candidateNodeID)
{
    vector<int> subgraph(templist);
    vector<int> connectedNodes = graph->getNeighbors(candidateNodeID);

    // calculate intersection of the neighbors of the candidate Node and the subgraph
    vector<int> intersectionSubgraphConnectedNodes;
    sort(subgraph.begin(), subgraph.end());
    sort(connectedNodes.begin(), connectedNodes.end());
    set_intersection(connectedNodes.begin(), connectedNodes.end(), subgraph.begin(), subgraph.end(), back_inserter(intersectionSubgraphConnectedNodes));

    return intersectionSubgraphConnectedNodes.size(); // size of intersections equals candidate participation
}

/**
 * performs l2 normalization in place on the vectors inside an int-vector map.
 *
 * @param embeddings[in, out] a map <int, vector<double>> containing embeddings for nodes
 */
void l2normalize(unordered_map<int, vector<double>> &embeddings)
{
    for (auto &[nodeID, embedding] : embeddings)
    {
        double norm = 0.0;

        // caculating norm factor
        for (double value : embedding)
        {
            norm += pow(value, 2);
        }
        norm = sqrt(norm);

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
int getAverageDegree(shared_ptr<Graph> graph)
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
void filterAndSort(vector<int> &list, vector<double> &scores, double threshhold)
{
    vector<pair<int, double>> idScorePairs;

    for (size_t i = 0; i < list.size(); ++i)
    {
        if (scores[i] >= threshhold)
        {
            idScorePairs.emplace_back(list[i], scores[i]);
        }
    }

    // Sort pairs based on score in descending order
    sort(idScorePairs.begin(), idScorePairs.end(),
              [](const pair<int, double> &a, const pair<int, double> &b)
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

