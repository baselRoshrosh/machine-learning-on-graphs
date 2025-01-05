#ifndef BASICEDGES_HPP
#define BASICEDGES_HPP

#include "IBasicEdges.hpp"
#include <vector>


class BasicEdges : public IBasicEdges
{
private:
    // Store edges as (row, col, value) with value (weight) 1
    std::vector<std::tuple<int, int, int>> edges;

public:
    // Constructor
    BasicEdges(const std::vector<std::pair<int, int>> &initialEdges)
    {
        for (const auto &edge : initialEdges)
        {
            addEdge(edge.first, edge.second);
        }
    };

    // Adds a new edge to the edge list
    void addEdge(int source, int destination) override
    {
        edges.emplace_back(source, destination, 1);
    };

    // Get neighbors of a given node
    std::vector<int> getNeighbors(int nodeID) override
    {
        std::vector<int> neighbors;
        for (const auto &[row, col, value] : edges)
        {
            if (row == nodeID)
            {
                neighbors.push_back(col);
            }
            else if (col == nodeID)
            {
                neighbors.push_back(row);
            }
        }
        return neighbors;
    }

    // Check if an edge exists
    bool isEdge(int source, int destination) override
    {
        for (const auto &[row, col, value] : edges)
        {
            if ((row == source && col == destination) ||
                (row == destination && col == source))
            {
                return true;
            }
        }
        return false;
    }

    // Retrieves all edges in the graph
    std::vector<std::pair<int, int, int>> getEdges() override
    {
        return edges;
    }

    // Returns the number of edges
    int size() override
    {
        return edges.size();
    }

    // Get neighbors of a given node
    std::vector<int> getNeighbors(int nodeID) override
    {
        std::vector<int> neighbors;
        for (const auto &[row, col, value] : edges)
        {
            if (row == nodeID)
            {
                neighbors.push_back(col);
            }
            else if (col == nodeID)
            {
                neighbors.push_back(row);
            }
        }
        return neighbors;
    }

    // Check if an edge exists
    bool isEdge(int source, int destination) override
    {
        for (const auto &[row, col, value] : edges)
        {
            if ((row == source && col == destination) ||
                (row == destination && col == source))
            {
                return true;
            }
        }
        return false;
    }

    std::vector < std::vector<int> getAdjancencyMatrix(int numNodes) const override
    {
        // initialize adjacency matrix, filled with 0
        std::vector<std::vector<int>> matrix(numNodes, std::vector<int>(numNodes, 0));

        for (const auto &[row, col, value] : edges)
        {
            // undirected
            matrix[row][col] = value; // from row to col
            matrix[col][row] = value; // from col to row
        }

        return matrix;
    }
};

#endif // BASICEDGES_HPP
