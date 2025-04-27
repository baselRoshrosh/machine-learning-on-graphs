# Semester Project WS24/25 — Machine Learning on Graphs

## Overview

This project explores **machine learning techniques on graphs** to predict missing node features.  
We developed a full C++/Python pipeline that implements and compares three different strategies:

- **k-Nearest Neighbors (KNN)** (baseline)
- **Topo2Vec** (topology-based embeddings)
- **Attributed DeepWalk (ADW)** (attribute-aware random walks)

The goal was to restore incomplete graph data using topological and attribute information.  
We integrated our C++ core with Python via **pybind11** to enable flexible experimentation and evaluation.

---

## Key Features

- 📈 **Feature Imputation** on large attributed graphs
- 🚀 **C++ core** with high-performance data structures (adjacency arrays, efficient sampling)
- 🔗 **Python integration** using pybind11
- 🔬 **Graph Embedding** via SkipGram (adapted for graph structures)
- 🧪 **Automated Testing and CI Pipelines** via GitLab CI
- 📊 **Evaluation on real-world datasets**: Amazon, CoraFull, GitHub, Twitch, Genius

---

## Project Structure

```
├── input/            # Datasets (edges and features)
├── src/              # Core C++ source code (Graph, Node, Strategies)
├── include/          # C++ headers
├── pybind/           # Python bindings
├── output/           # Output files with imputed features
├── scripts/          # Evaluation and helper scripts
├── CMakeLists.txt    # Build configuration
└── .gitlab-ci.yml    # CI/CD pipeline configuration (GitLab)
```

---

## Strategies

### 🔹 k-Nearest Neighbors (KNN)
- Baseline method.
- Predicts missing features by averaging over the `k` nearest neighbors.
- Graph topology used directly for neighbor selection (BFS search).

### 🔹 Topo2Vec
- Embedding-based method using **context subgraphs**.
- Neighborhood Affinity (NA) and Subgraph Affinity (SA) scores used for context selection.
- Embeddings learned using a graph-adapted SkipGram model.

### 🔹 Attributed DeepWalk (ADW)
- Random walks biased by **both** topological and attribute similarities.
- Edge weights calculated using a fusion of feature similarity (cosine similarity) and structural similarity (Jaccard index).
- Transition probabilities managed via **Alias Tables** for efficient sampling.

---

## Technologies Used

- **C++17** — main project language
- **Python 3** — scripting and evaluation
- **pybind11** — C++ to Python interface
- **CMake** — build system
- **GitLab CI/CD** — continuous integration
- **Git LFS** — large file management (datasets)

---

## Installation and Usage

```bash
# Clone the repository
git clone https://github.com/baselRoshrosh/semesterprojekt_ws2425.git
cd semesterprojekt_ws2425

# Create a build directory
mkdir build && cd build

# Configure and build
cmake ..
make

# Run experiments or use the Python bindings
python scripts/run_experiment.py
```

---

## Results

We evaluated the strategies across several datasets. Key observations:

- **KNN**: Fast and stable baseline, good performance on simpler datasets.
- **Topo2Vec**: Strong topological feature recovery, but high computational cost.
- **ADW**: Conceptually promising (combining structure and attributes), but runtime issues limited final evaluation.

Despite technical challenges, the project delivered valuable insights into the effectiveness of graph-based learning techniques.

---

## Challenges and Learnings

- Implementing and optimizing graph data structures (Adjacency arrays, Alias sampling)
- Handling large datasets with Git LFS
- Dealing with C++ memory management issues (e.g., segmentation faults)
- Managing a full CI/CD pipeline for C++ and Python hybrid projects
- Collaboration under difficult conditions (remote work, sickness, tight deadlines)

---

## Acknowledgements

Project completed as part of the **"Machine Learning on Graphs"** course at **Humboldt University of Berlin** (Winter Semester 2024/25).

Team Members:
- Basel Roshrosh
- Moritz Spühler
- Leon Behrndt
- Johannes Storck

Special thanks to Prof. Dr. Frank Brandt for supervision and support.

---

## Sources

- [Node2Vec: Scalable Feature Learning for Networks](https://dl.acm.org/doi/10.1145/2939672.2939754)
- [Efficient Estimation of Word Representations in Vector Space (SkipGram/Word2Vec)](https://arxiv.org/abs/1301.3781)
- [A Modified DeepWalk Method for Link Prediction in Attributed Social Networks](https://link.springer.com/article/10.1007/s00607-021-00982-2)
- [k-Nearest Neighbor Learning with Graph Neural Networks](https://www.mdpi.com/2227-7390/9/8/830)
- [Topo2Vec: A Novel Node Embedding Generation Based on Network Topology for Link Prediction]([topo2vec.pdf](https://github.com/user-attachments/files/19931214/topo2vec.pdf)

---

## License

This project is for educational purposes.  
For any questions or further use, please contact the authors.


