# run this in your build folder to ensure the module is usable:
# cmake -DBUILD_TESTS=OFF && make && cd .. && pip install .
import semProject 
import os

input_dir = os.path.join("input", "cornell")
edges_file = os.path.join(input_dir, "cornell_edges.txt")  
nodes_file = os.path.join(input_dir, "cornell_mcar_0.5.txt")  

# Check if files exist
if not (os.path.exists(edges_file) and os.path.exists(nodes_file)):
    print("Error: Input files not found!")
    exit(1)

# Test KNN
print("Testing KNN...")
graph = semProject.Graph(nodes_file, edges_file)
knn = semProject.KNN(graph)
# knn.configure()  # Configure if needed
knn.run()
results_knn = knn.extract_results()
knn.save_features(graph, os.path.join("output", "cornell_features_knn.txt"))


# Test Topo2Vec
print("Testing Topo2Vec...")
graph = semProject.Graph(nodes_file, edges_file)
topo2vec = semProject.Topo2Vec(graph)
# topo2vec.configure()  # Configure if needed
topo2vec.run()
results_t2v = topo2vec.extract_results()
knn.save_features(graph, os.path.join("output", "cornell_features_topo2vec.txt"))

# Test AttributedDeepwalk
print("Testing AttributedDeepwalk...")
graph = semProject.Graph(nodes_file, edges_file)
deepwalk = semProject.AttributedDeepwalk(graph)
# deepwalk.configure()  # Configure if needed
deepwalk.run()
results_dw = deepwalk.extract_results()
knn.save_features(graph, os.path.join("output", "cornell_features_adw.txt"))

print("All tests completed successfully!")