#include "../include/BasicEdges.hpp"
#include <vector>

class BasicEdges : public IBasicEdges
{
private:
    // Store edges as (row, col, value) with value (weight) 1
    std::vector<std::tuple<int, int, int>> edges;

public:
    // Constructor
    BasicEdges(const std::vector<std::tuple<int, int>> &initialEdges)
    {
        for (const auto &edge : initialEdges)
        {
            addEdge(std::get<0>(edge), std::get<1>(edge));
        }
    };

    // Adds a new edge to the edge list
    void addEdge(int source, int destination)
    {
        edges.emplace_back(source, destination, 1);
    };

    // Get neighbors of a given node
    std::vector<int> getNeighbors(int nodeID) const
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
    bool isEdge(int source, int destination) const
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
    std::vector<std::tuple<int, int, int>> getEdges() const
    {
        return edges;
    }

    // Returns the number of edges
    int size() const
    {
        return edges.size();
    }

    // Get neighbors of a given node
    std::vector<int> getNeighbors(int nodeID) const
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

    std::vector<std::vector<int>> getAdjacencyMatrix(int numNodes) const
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