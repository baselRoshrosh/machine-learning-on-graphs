import simProject  
import os

# Define input file paths
edges_file = os.path.join("input", "twitch_edges.txt")  # Replace with actual file name
nodes_file = os.path.join("input", "twitch_nodes.txt")  # Replace with actual file name

# Check if files exist
if not (os.path.exists(edges_file) and os.path.exists(nodes_file)):
    print("Error: Input files not found!")
    exit(1)

# Load graph (Assuming you have a Graph class or method to read files)
graph = simProject.Graph()
graph.load_from_files(edges_file, nodes_file)  # Replace with actual method

# Test AttributedDeepwalk
#print("Testing AttributedDeepwalk...")
#deepwalk = simProject.AttributedDeepwalk(graph)
#deepwalk.configure()  # Configure if needed
#deepwalk.run()
#results_dw = deepwalk.extract_results()
#print("AttributedDeepwalk Results:", results_dw)

# Test KNN
print("Testing KNN...")
knn = simProject.KNN(graph)
knn.configure()  # Configure if needed
knn.run()
results_knn = knn.extract_results()
print("KNN Results:", results_knn)

# Test Topo2Vec
print("Testing Topo2Vec...")
topo2vec = simProject.Topo2Vec(graph)
topo2vec.configure()  # Configure if needed
topo2vec.run()
results_t2v = topo2vec.extract_results()
print("Topo2Vec Results:", results_t2v)

print("All tests completed successfully!")