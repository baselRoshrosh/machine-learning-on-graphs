# Semesterprojekt_WS2425

## Project Overview

This project integrates various graph-based algorithms using Python bindings for C++ implementations. The main algorithms included are Attributed DeepWalk, kNN, and Topo2Vec.

## Getting Started

To get started with this project, follow the steps below.

### Prerequisites

- Python 3.8 or higher
- C++17 compatible compiler
- CMake 3.14 or higher
- [pybind11](https://github.com/pybind/pybind11)

### Installation

1. Clone the repository:

    ```sh
    git clone https://gitlab.informatik.hu-berlin.de/spuehlem/semesterprojekt_ws2425.git
    cd semesterprojekt_ws2425
    ```

2. Build the project:

    ```sh
    mkdir build
    cd build
    cmake ..
    make
    ```

3. Install the Python package:

    ```sh
    cd .. # (back to root dir)
    pip install .
    ```

### Usage

Here are examples of how to use the provided algorithms.

#### Attributed DeepWalk

```python
import semProject

graph = semProject.Graph("path/to/nodes_file.txt", "path/to/edges_file.txt")
deepwalk = semProject.AttributedDeepwalk(graph)
deepwalk.configure(walkLength=20, walksPerNode=10)  # Configure if needed
deepwalk.run()
results_dw = deepwalk.extract_results()
saveFeatures(graph, "tmp/adw_results.txt")
```

#### kNN

```python
import semProject

graph = semProject.Graph("path/to/nodes_file.txt", "path/to/edges_file.txt")
knn = semProject.KNN(graph)
knn.configure(walkLength=20, walksPerNode=10)  # Configure if needed
knn.run()
results_knn = knn.extract_results()
saveFeatures(graph, "tmp/knn_results.txt")
```

#### Topo2Vec

```python
import semProject

graph = semProject.Graph("path/to/nodes_file.txt", "path/to/edges_file.txt")
topo2vec = semProject.Topo2Vec(graph)
topo2vec.configure(walkLength=20, walksPerNode=10)  # Configure if needed
topo2vec.run()
results_t2v = topo2vec.extract_results()
saveFeatures(graph, "tmp/topo2vec_results.txt")
```

## Authors and Acknowledgments

- **Moritz Spühler, Basel Roshrosh, Johannes Storck, Leon Behrndt** 
