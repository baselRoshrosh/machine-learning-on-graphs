# run this in your build folder to ensure the module is usable:
# cmake -DBUILD_TESTS=OFF && make && cd .. && pip install .
import semProject 
import os

input_dir = os.path.join("input", "cornell")
edges_file = os.path.join(input_dir, "cornell_edges.txt")  
nodes_file = os.path.join(input_dir, "cornell_nodes.txt")  

# Check if files exist
if not (os.path.exists(edges_file) and os.path.exists(nodes_file)):
    print("Error: Input files not found!")
    exit(1)

graph = semProject.Graph(nodes_file, edges_file)

# Test AttributedDeepwalk
#print("Testing AttributedDeepwalk...")
#deepwalk = semProject.AttributedDeepwalk(graph)
#deepwalk.configure()  # Configure if needed
#deepwalk.run()
#results_dw = deepwalk.extract_results()
#print("AttributedDeepwalk Results:", results_dw)

# Test KNN
print("Testing KNN...")
knn = semProject.KNN(graph)
knn.configure()  # Configure if needed
knn.run()
results_knn = knn.extract_results()
print("KNN Results:", results_knn)

# Test Topo2Vec
print("Testing Topo2Vec...")
topo2vec = semProject.Topo2Vec(graph)
topo2vec.configure()  # Configure if needed
topo2vec.run()
results_t2v = topo2vec.extract_results()
print("Topo2Vec Results:", results_t2v)

print("All tests completed successfully!")