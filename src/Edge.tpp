#include <IEdge.hpp>

/**
 * @class Edge
 * @brief Concrete implementation of the IEdge interface.
 */
class Edge : public IEdge
{
public:
    Edge(int source, int destination)
    {
        this->source = source;
        this->destination = destination;
    }

    int getsource() const override
    {
        return this->source;
    }

    int getdestination() const override
    {
        return this->destination;
    }
};
