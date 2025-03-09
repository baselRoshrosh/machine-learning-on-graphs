import pytest
import semProject
from . import GRAPH_PATHS

def test_topo2vec():
    # Initialize the Graph
    graph = semProject.Graph(GRAPH_PATHS["nodes"], GRAPH_PATHS["edges"])
    
    # Initialize Topo2Vec with the graph
    strategy = semProject.Topo2Vec(graph)
    
    # Run the strategy
    strategy.run()
    
    # Extract results and check the expected output
    results = strategy.extract_results()
    assert results is not None

def test_topo2vec_save_features():
    # Initialize the Graph
    graph = semProject.Graph(GRAPH_PATHS["nodes"], GRAPH_PATHS["edges"])
    
    # Initialize Topo2Vec with the graph
    strategy = semProject.Topo2Vec(graph)
    
    # Run the strategy
    strategy.run()
    
    # Save features and check if the file exists
    strategy.save_features("topo2vec_output.txt")
    with open("topo2vec_output.txt", "r") as f:
        content = f.read()
    assert len(content) > 0  # Ensure the file has content
