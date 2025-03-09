#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <memory>

#include "Graph.hpp"
#include "AttributedDeepwalk.hpp"
#include "KNN.hpp"
#include "Topo2Vec.hpp"
#include "StrategyRunner.hpp"

using namespace std;
namespace py = pybind11;

PYBIND11_MODULE(semProject, m)
{
    m.doc() = R"pbdoc(
        Python Bindings for Attributed DeepWalk, kNN, and Topo2Vec.
        
        This module provides access to various network embedding strategies, 
        allowing Python users to apply and configure these methods on graph data.
    )pbdoc";

    py::class_<Graph, shared_ptr<Graph>>(m, "Graph", R"pbdoc(
        Represents a graph structure loaded from node and edge files.
        
        Attributes:
            nodesFile (str): Path to the file containing node information.
            edgesFile (str): Path to the file containing edge information.
    )pbdoc")
        .def(py::init<const string &, const string &>(), py::arg("nodesFile"), py::arg("edgesFile"))
        .def("get_node_count", &Graph::getNodeCount, R"pbdoc(
            Retrieves node count of the graph.
            
            Returns:
                int: Number of nodes in the graph.
        )pbdoc")
        .def("get_edge_count", &Graph::getEdgeCount, R"pbdoc(
            Retrieves edge count of the graph.
            
            Returns:
                int: Number of edges in the graph.
        )pbdoc");

    py::class_<StrategyRunner<AttributedDeepwalk>>(m, "AttributedDeepwalk", R"pbdoc(
        Implements the Attributed DeepWalk algorithm for node embeddings.

        This method extends DeepWalk by incorporating node attributes, 
        enabling better embeddings for attributed graphs.
    )pbdoc")
        .def(py::init<shared_ptr<Graph>>(), py::arg("graph"))
        .def("run", &StrategyRunner<AttributedDeepwalk>::run, R"pbdoc(
            Executes the Attributed DeepWalk algorithm on the graph.
            
            Returns:
                None
        )pbdoc")
        .def("extract_results", &StrategyRunner<AttributedDeepwalk>::extractResults, R"pbdoc(
            Extracts and returns the computed embeddings.

            Returns:
                Graph: The graph with its missing features filled out 
        )pbdoc")
        .def("configure", &StrategyRunner<AttributedDeepwalk>::configure, R"pbdoc(
            Configures the algorithm parameters.

            Parameters:
                config (dict): Configuration settings for Attributed DeepWalk.
            
            Returns:
                None
        )pbdoc")
        .def("reset", &StrategyRunner<AttributedDeepwalk>::reset, R"pbdoc(
            Resets all configurations to default values.
            
            Returns:
                None
        )pbdoc")
        .def("save_features", &StrategyRunner<AttributedDeepwalk>::saveFeatures, R"pbdoc(
            Saves the computed features in the original format used by the algorithm.
            
            Returns:
                None
        )pbdoc");

    py::class_<StrategyRunner<KNN>>(m, "KNN", R"pbdoc(
        Implements k-Nearest Neighbors (kNN) for node classification.

        This method predicts missing attributes based on nearest neighbors 
        in the embedding space.
    )pbdoc")
        .def(py::init<shared_ptr<Graph>>(), py::arg("graph"))
        .def("run", &StrategyRunner<KNN>::run, R"pbdoc(
            Runs the kNN classification on the graph.
            
            Returns:
                None
        )pbdoc")
        .def("extract_results", &StrategyRunner<KNN>::extractResults, R"pbdoc(
            Extracts classification results.

            Returns:
                Graph: The graph with its missing features filled out 
        )pbdoc")
        .def("configure", &StrategyRunner<KNN>::configure, R"pbdoc(
            Configures kNN-specific parameters.

            Parameters:
                config (dict): Configuration settings for kNN.
            
            Returns:
                None
        )pbdoc")
        .def("reset", &StrategyRunner<KNN>::reset, R"pbdoc(
            Resets kNN parameters to default values.
            
            Returns:
                None
        )pbdoc")
        .def("save_features", &StrategyRunner<KNN>::saveFeatures, R"pbdoc(
            Saves the computed features in the original format used by the algorithm.
            
            Returns:
                None
        )pbdoc");

    py::class_<StrategyRunner<Topo2Vec>>(m, "Topo2Vec", R"pbdoc(
        Implements the Topo2Vec method for structural node embeddings.

        This method generates embeddings based on topological features of the graph.
    )pbdoc")
        .def(py::init<shared_ptr<Graph>>(), py::arg("graph"))
        .def("run", &StrategyRunner<Topo2Vec>::run, R"pbdoc(
            Executes the Topo2Vec embedding algorithm.
            
            Returns:
                None
        )pbdoc")
        .def("extract_results", &StrategyRunner<Topo2Vec>::extractResults, R"pbdoc(
            Extracts the generated embeddings.

            Returns:
                Graph: The graph with its missing features filled out 
        )pbdoc")
        .def("configure", &StrategyRunner<Topo2Vec>::configure, R"pbdoc(
            Configures Topo2Vec parameters.

            Parameters:
                config (dict): Configuration settings for Topo2Vec.
            
            Returns:
                None
        )pbdoc")
        .def("reset", &StrategyRunner<Topo2Vec>::reset, R"pbdoc(
            Resets Topo2Vec parameters to default values.
            
            Returns:
                None
        )pbdoc")
        .def("save_features", &StrategyRunner<Topo2Vec>::saveFeatures, R"pbdoc(
            Saves the computed features in the original format used by the algorithm.
            
            Returns:
                None
        )pbdoc");
}
