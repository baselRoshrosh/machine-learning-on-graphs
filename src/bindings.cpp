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
    m.doc() = "Python Bindings for Attributed DeepWalk, kNN and Topo2Vec";

    py::class_<Graph, shared_ptr<Graph>>(m, "Graph")
        .def(py::init<const string &, const string &>(), py::arg("nodesFile"), py::arg("edgesFile"));

    py::class_<StrategyRunner<AttributedDeepwalk>>(m, "AttributedDeepwalk")
        .def(py::init<shared_ptr<Graph>>(), py::arg("graph"))
        .def("run", &StrategyRunner<AttributedDeepwalk>::run, "runs Attributed DeepWalk")
        .def("extract_results", &StrategyRunner<AttributedDeepwalk>::extractResults, "extracts results")
        .def("configure", &StrategyRunner<AttributedDeepwalk>::configure, "configure parameters")
        .def("reset", &StrategyRunner<AttributedDeepwalk>::reset, "resets all configurations")
        .def("saveFeatures", &StrategyRunner<AttributedDeepwalk>::saveFeatures, "saves Features as in original format");

    py::class_<StrategyRunner<KNN>>(m, "KNN")
        .def(py::init<shared_ptr<Graph>>(), py::arg("graph"))
        .def("run", &StrategyRunner<KNN>::run, "runs kNN")
        .def("extract_results", &StrategyRunner<KNN>::extractResults, "extracts results")
        .def("configure", &StrategyRunner<KNN>::configure, "configure kNN-parameters")
        .def("reset", &StrategyRunner<KNN>::reset, "resets all configurations")
        .def("saveFeatures", &StrategyRunner<KNN>::saveFeatures, "saves Features as in original format");

    py::class_<StrategyRunner<Topo2Vec>>(m, "Topo2Vec")
        .def(py::init<shared_ptr<Graph>>(), py::arg("graph"))
        .def("run", &StrategyRunner<Topo2Vec>::run, "runs Topo2Vec")
        .def("extract_results", &StrategyRunner<Topo2Vec>::extractResults, "extracts results")
        .def("configure", &StrategyRunner<Topo2Vec>::configure, "configure parameters")
        .def("reset", &StrategyRunner<Topo2Vec>::reset, "resets all configurations")
        .def("saveFeatures", &StrategyRunner<Topo2Vec>::saveFeatures, "saves Features as in original format");
}
