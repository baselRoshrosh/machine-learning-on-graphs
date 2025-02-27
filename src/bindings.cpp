#include <pybind11/pybind11.h>
#include <pybind11/stl.h>  // Falls STL-Container wie std::vector oder std::unordered_map genutzt werden

#include "AttributedDeepwalk.hpp"
#include "KNN.hpp"
#include "Topo2Vec.hpp"

namespace py = pybind11;

PYBIND11_MODULE(mein_modul, m) {
    m.doc() = "Python Bindings für Attributed DeepWalk, kNN und Topo2Vec";

    // Klasse AttributedDeepwalk
    py::class_<AttributedDeepwalk>(m, "AttributedDeepwalk")
        .def(py::init<std::shared_ptr<Graph>>())  // Falls du einen Konstruktor mit Graph nutzt
        .def("run", &AttributedDeepwalk::run, "Führt Attributed DeepWalk aus")
        .def("extract_results", &AttributedDeepwalk::extractResults, "Extrahiert die Ergebnisse")
        .def("configure", &AttributedDeepwalk::configure, "Konfiguriert die Strategie")
        .def("reset", &AttributedDeepwalk::reset, "Setzt den Zustand zurück");

    // Klasse KNN
    py::class_<KNN>(m, "KNN")
        .def(py::init<std::shared_ptr<Graph>>())  // Falls du einen Konstruktor mit Graph nutzt
        .def("run", &KNN::run, "Führt kNN-Suche aus")
        .def("extract_results", &KNN::extractResults, "Extrahiert die Ergebnisse")
        .def("configure", &KNN::configure, "Konfiguriert kNN-Parameter")
        .def("reset", &KNN::reset, "Setzt den Zustand zurück");

    // Klasse Topo2Vec
    py::class_<Topo2Vec>(m, "Topo2Vec")
        .def(py::init<std::shared_ptr<Graph>>())  // Falls du einen Konstruktor mit Graph nutzt
        .def("run", &Topo2Vec::run, "Führt Topo2Vec aus")
        .def("extract_results", &Topo2Vec::extractResults, "Extrahiert die Ergebnisse")
        .def("configure", &Topo2Vec::configure, "Konfiguriert Topo2Vec-Parameter")
        .def("reset", &Topo2Vec::reset, "Setzt den Zustand zurück");
}
