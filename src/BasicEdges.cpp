#include "../include/BasicEdges.hpp"
#include <vector>

class BasicEdges : public IBasicEdges
{
private:
    // Store edges as (row, col, value) with value (weight) 1
    std::vector<std::tuple<int, int, int>> edges;

public:
    /**
     * @brief Constructor to initialize edges with a predefined set of edges.
     *
     * @param initialEdges A vector of edges to initialize the graph.
     */
    BasicEdges(const std::vector<std::tuple<int, int>> &initialEdges)
    {
        for (const auto &edge : initialEdges)
        {
            addEdge(std::get<0>(edge), std::get<1>(edge));
        }
    };

    void addEdge(int source, int destination) override
    {
        edges.emplace_back(source, destination, 1);
    };

    std::vector<int> getNeighbors(int nodeID) const override
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

    bool isEdge(int source, int destination) const override
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

    std::vector<std::tuple<int, int, int>> getEdges() const override
    {
        return edges;
    }

    int size() const override
    {
        return edges.size();
    }

    std::vector<int> getNeighbors(int nodeID) const override
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

    std::vector<std::vector<int>> getAdjacencyMatrix(int numNodes) const override
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