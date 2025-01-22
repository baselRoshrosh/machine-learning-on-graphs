#include "../include/IComplexEdges.hpp"
#include <unordered_map>

class SparseMatrixEdges : public IComplexEdges
{
private:
    // Sparse matrix to store edges as adjacency list with weights
    std::unordered_map<int, std::unordered_map<int, int>> sparseMatrix;

public:
    // Constructor
    SparseMatrixEdges(const std::vector<std::pair<int, int>> &initialEdges)
    {
        for (const auto &edge : initialEdges)
        {
            addEdge(edge.first, edge.second);
        }
    }

    // Adds a new edge to the sparse matrix
    void addEdge(int source, int destination) override
    {
        sparseMatrix[source][destination] = 1;
        sparseMatrix[destination][source] = 1; // Assuming undirected graph
    }

    // Retrieves the neighbors of a given node
    std::vector<int> getNeighbors(int nodeID) const override
    {
        std::vector<int> neighbors;
        if (sparseMatrix.find(nodeID) != sparseMatrix.end())
        {
            for (const auto &[neighbor, weight] : sparseMatrix.at(nodeID))
            {
                neighbors.push_back(neighbor);
            }
        }
        return neighbors;
    }

    // Checks if an edge exists between two nodes
    bool isEdge(int source, int destination) const override
    {
        return sparseMatrix.at(source).find(destination) != sparseMatrix.at(source).end();
    }

    // Retrieves all edges in the graph
    std::vector<std::tuple<int, int, int>> getEdges() const override
    {
        std::vector<std::tuple<int, int, int>> edges;
        for (const auto &[source, neighbors] : sparseMatrix)
        {
            for (const auto &[destination, weight] : neighbors)
            {
                if (source < destination) // To avoid duplicate edges
                {
                    edges.emplace_back(source, destination, weight);
                }
            }
        }
        return edges;
    }

    // Returns the number of edges
    int size() const override
    {
        int edgeCount = 0;
        for (const auto &[source, neighbors] : sparseMatrix)
        {
            edgeCount += neighbors.size();
        }
        return edgeCount / 2; // Since the graph is undirected
    }

    // Retrieves the adjacency matrix of the graph
    std::vector<std::vector<int>> getAdjacencyMatrix(int numNodes) const override
    {
        std::vector<std::vector<int>> matrix(numNodes, std::vector<int>(numNodes, 0));
        for (const auto &[row, neighbors] : sparseMatrix)
        {
            for (const auto &[col, weight] : neighbors)
            {
                matrix[row][col] = weight;
            }
        }
        return matrix;
    }
};
