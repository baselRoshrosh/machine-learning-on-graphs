#include <pybind11/pybind11.h>
#include <pybind11/stl.h> 
#include <memory> 

#include "Graph.hpp"
#include "AttributedDeepwalk.hpp"
#include "KNN.hpp"
#include "Topo2Vec.hpp"

namespace py = pybind11;

PYBIND11_MODULE(semProject, m) {
    m.doc() = "Python Bindings for Attributed DeepWalk, kNN and Topo2Vec";

    py::class_<Graph, std::shared_ptr<Graph>>(m, "Graph")
        .def(py::init<>())  
        .def("load_from_files", &Graph::loadFromFiles, "Loads graph from edge and node files.");

    py::class_<AttributedDeepwalk>(m, "AttributedDeepwalk")
        .def(py::init<std::shared_ptr<Graph>>(), py::arg("graph"))
        .def("run", &AttributedDeepwalk::run, "runs Attributed DeepWalk")
        .def("extract_results", &AttributedDeepwalk::extractResults, "extracs results")
        .def("configure", &AttributedDeepwalk::configure, "configure parameters")
        .def("reset", &AttributedDeepwalk::reset, "resets all configurations");

    py::class_<KNN>(m, "KNN")
        .def(py::init<std::shared_ptr<Graph>>(), py::arg("graph")) 
        .def("run", &KNN::run, "runs kNN")
        .def("extract_results", &KNN::extractResults, "extracs results")
        .def("configure", &KNN::configure, "configure kNN-parameters")
        .def("reset", &KNN::reset, "resets all configurations");

    py::class_<Topo2Vec>(m, "Topo2Vec")
        .def(py::init<std::shared_ptr<Graph>>(), py::arg("graph")) 
        .def("run", &Topo2Vec::run, "runs Topo2Vec")
        .def("extract_results", &Topo2Vec::extractResults, "extracs results")
        .def("configure", &Topo2Vec::configure, "configure parameters")
        .def("reset", &Topo2Vec::reset, "resets all configurations");
}
