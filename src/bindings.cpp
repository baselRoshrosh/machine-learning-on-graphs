#include <pybind11/pybind11.h>
#include <pybind11/stl.h>  

#include "AttributedDeepwalk.hpp"
#include "KNN.hpp"
#include "Topo2Vec.hpp"

namespace py = pybind11;

PYBIND11_MODULE(semProject, m) {
    m.doc() = "Python Bindings for Attributed DeepWalk, kNN and Topo2Vec";

    py::class_<AttributedDeepwalk>(m, "AttributedDeepwalk")
        .def(py::init<std::shared_ptr<Graph>>())
        .def("run", &AttributedDeepwalk::run, "runsAttributed DeepWalk")
        .def("extract_results", &AttributedDeepwalk::extractResults, "extracs results")
        .def("configure", &AttributedDeepwalk::configure, "configure parameters")
        .def("reset", &AttributedDeepwalk::reset, "resets all configurations");

    py::class_<KNN>(m, "KNN")
        .def(py::init<std::shared_ptr<Graph>>())  
        .def("run", &KNN::run, "runs kNN")
        .def("extract_results", &KNN::extractResults, "extracs results")
        .def("configure", &KNN::configure, "configure kNN-parameters")
        .def("reset", &KNN::reset, "resets all configurations");

    py::class_<Topo2Vec>(m, "Topo2Vec")
        .def(py::init<std::shared_ptr<Graph>>())  
        .def("run", &Topo2Vec::run, "Führt Topo2Vec aus")
        .def("extract_results", &Topo2Vec::extractResults, "extracs results")
        .def("configure", &Topo2Vec::configure, "configure parameters")
        .def("reset", &Topo2Vec::reset, "resets all configurations");
}
