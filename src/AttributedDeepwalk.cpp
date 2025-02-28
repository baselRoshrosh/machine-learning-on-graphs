#include "AttributedDeepwalk.hpp"

#include <set>
#include <algorithm>
#include <unordered_set>
#include <queue>

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
    // TODO
}

double AttributedDeepwalk::measuring_attribute_similarity(int node1, int node2) const
{
    double similarity;
    // TODO
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
    std::set_union(coverNode1.begin(), coverNode1.end(), coverNode2.begin(), coverNode2.end(), std::inserter(intersectionOfCovers, intersectionOfCovers.begin()));

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