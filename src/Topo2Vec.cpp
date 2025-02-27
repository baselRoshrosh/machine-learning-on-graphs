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
void updateEmbeddings(std::unordered_map<int, std::vector<double>> &, int, int, int, double, double);
std::vector<int> getNegativeSamples(std::shared_ptr<Graph>, int, int);
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
 * ======= getSample() ===============
 */

std::vector<std::vector<double>> Topo2Vec::getSample(const std::vector<std::vector<double>> &setOfVectors, int sampleSize)
{
    std::vector<std::vector<double>> sample;
    if (setOfVectors.empty() || sampleSize <= 0)
    { // Out-of-bounds check
        return sample;
    }
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, setOfVectors.size() - 1);

    for (int i = 0; i < sampleSize; i++)
    {
        sample.push_back(setOfVectors[dis(gen)]);
    }

    return sample;
}

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

void Topo2Vec::skipGram(std::unordered_map<int, std::vector<double>> &embeddings, const std::vector<std::vector<int>> &subGraphs)
{
    for (int epoch = 0; epoch < numEpochs; epoch++) // do the descent epoch-times
    {
        for (const auto &subGraph : subGraphs)
        {
            for (size_t i = 0; i < subGraph.size(); i++)
            {
                int targetNode = subGraph[i];

                // get context nodes similar to context words in word2vec by creating a "window" of nodes around the target
                for (int j = -windowSize; j <= windowSize; j++)
                {
                    if (j == 0 || (i + j) < 0 || (i + j) >= subGraph.size())
                        continue;

                    int contextNode = subGraph[i + j];

                    // positive example (real edge)
                    updateEmbeddings(embeddings, embeddingDimensions, targetNode, contextNode, 1, learningRate);

                    // negative samples
                    std::vector<int> negativeSamples = getNegativeSamples(graph, targetNode, numNegativeSamples);
                    for (int negativeNode : negativeSamples)
                    {
                        updateEmbeddings(embeddings, embeddingDimensions, targetNode, negativeNode, 0, learningRate);
                    }
                }
            }
        }
    }
}

std::vector<std::vector<double>> Topo2Vec::getSimilarNodes(
    const std::unordered_map<int, std::vector<double>> &embeddings,
    const std::vector<double> &queryVector,
    int kSimilarNodes)
{
    if (embeddings.empty() || queryVector.empty() || kSimilarNodes <= 0)
    {
        return {};
    }

    // Calculate the L2 norm (Euclidean norm) of the query vector.
    double sumQuery = 0.0;
    for (double value : queryVector)
    {
        sumQuery += value * value;
    }
    double normQuery = std::sqrt(sumQuery);
    if (normQuery == 0)
    {
        return {};
    }

    // Define a min-heap (priority queue) to store pairs of (cosine similarity, nodeID)
    using SimilarityPair = std::pair<double, int>;
    std::priority_queue<SimilarityPair, std::vector<SimilarityPair>, std::greater<>> minHeap;

    // Iterate over all embeddings.
    for (const auto &[nodeID, embedding] : embeddings)
    {
        // Skip if dimensions do not match.
        if (embedding.size() != queryVector.size())
            continue;

        // Skip if this embedding is exactly the same as the query vector.
        if (embedding == queryVector)
            continue;

        // Compute dot product and the norm for the candidate embedding.
        double dot = 0.0, sumCandidate = 0.0;
        for (size_t i = 0; i < embedding.size(); i++)
        {
            dot += queryVector[i] * embedding[i];
            sumCandidate += embedding[i] * embedding[i];
        }
        double normCandidate = std::sqrt(sumCandidate);
        if (normCandidate == 0)
            continue;

        // Calculate cosine similarity.
        double cosineSimilarity = dot / (normQuery * normCandidate);

        // Maintain the top-k similar nodes using the min-heap.
        if (minHeap.size() < static_cast<size_t>(kSimilarNodes))
        {
            minHeap.emplace(cosineSimilarity, nodeID);
        }
        else if (cosineSimilarity > minHeap.top().first)
        {
            minHeap.pop();
            minHeap.emplace(cosineSimilarity, nodeID);
        }
    }

    // Extract the embeddings of the top-k similar nodes.
    std::vector<std::vector<double>> topKSimilar;
    while (!minHeap.empty())
    {
        int nodeID = minHeap.top().second;
        minHeap.pop();
        topKSimilar.push_back(embeddings.at(nodeID));
    }

    // Reverse so that the highest similarity comes first.
    std::reverse(topKSimilar.begin(), topKSimilar.end());

    return topKSimilar;
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
 * updates Embeddings based on the connection of two nodes
 *
 * @param embeddings[in, out] pointer to the embeddings<nodeID, embedding>
 * @param dimensions[in] how many dimensions an embedding has
 * @param targetNode[in]
 * @param contextNode[in]
 * @param label[in] whether it is a positive (1) or negative (0) example
 * @param learningRate[in] how large the gradient descent steps should be
 */
void updateEmbeddings(std::unordered_map<int, std::vector<double>> &embeddings, int dimensions, int targetNode, int contextNode, double label, double learningRate)
{
    std::vector<double> &targetVec = embeddings[targetNode];
    std::vector<double> &contextVec = embeddings[contextNode];

    // calculate gradient descent parameters
    double score = sigmoid(dotProduct(targetVec, contextVec));
    double gradient = (label - score) * learningRate;

    // update both of the embeddings
    for (size_t i = 0; i < dimensions; i++)
    {
        double temp = targetVec[i]; // Store original
        targetVec[i] += gradient * contextVec[i];
        contextVec[i] += gradient * temp;
    }
}

/**
 * gets a number of negative samples for a specified node
 *
 * @param graph the full graph
 * @param excludeNode the node to get negative samples for
 * @param numSamples how many negative samples should be created
 *
 * @return
 */
std::vector<int> getNegativeSamples(std::shared_ptr<Graph> graph, int excludeNode, int numSamples)
{
    std::vector<int> negativeSamples;
    std::vector<int> nodes = graph->getNodes();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, nodes.size() - 1);

    // get numSamples many samples that differ from the specified node
    while (negativeSamples.size() < numSamples)
    {
        int sampledNode = nodes[dist(gen)];
        if (sampledNode != excludeNode)
        {
            negativeSamples.push_back(sampledNode);
        }
    }

    return negativeSamples;
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
