import pytest
import semProject
from . import GRAPH_PATHS

def test_knn():
    # Initialize the Graph
    graph = semProject.Graph(GRAPH_PATHS["nodes"], GRAPH_PATHS["edges"])
    
    # Initialize KNN with the graph
    strategy = semProject.KNN(graph)
    
    # Run the strategy
    strategy.run()
    
    # Extract results and check the expected output
    results = strategy.extract_results()
    assert results is not None  # Modify as per your expectations

def test_knn_save_features():
    # Initialize the Graph
    graph = semProject.Graph(GRAPH_PATHS["nodes"], GRAPH_PATHS["edges"])
    
    # Initialize KNN with the graph
    strategy = semProject.KNN(graph)
    
    # Run the strategy
    strategy.run()
    
    # Save features and check if the file exists
    strategy.save_features("knn_output.txt")
    with open("knn_output.txt", "r") as f:
        content = f.read()
    assert len(content) > 0  # Ensure the file has content
