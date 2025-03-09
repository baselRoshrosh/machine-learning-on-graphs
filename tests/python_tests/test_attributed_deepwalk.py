import pytest
import semProject
from . import GRAPH_PATHS

def test_attributed_deepwalk():
    # Initialize the Graph
    graph = semProject.Graph(GRAPH_PATHS["nodes"], GRAPH_PATHS["edges"])
    
    # Initialize AttributedDeepwalk with the graph
    strategy = semProject.AttributedDeepwalk(graph)
    
    # Run the strategy
    strategy.run()

    # Extract results and check the expected output
    results = strategy.extract_results()
    assert results is not None  # Check if the results are not None, modify according to your expectations

def test_attributed_deepwalk_save_features():
    # Initialize the Graph
    graph = semProject.Graph(GRAPH_PATHS["nodes"], GRAPH_PATHS["edges"])
    
    # Initialize AttributedDeepwalk with the graph
    strategy = semProject.AttributedDeepwalk(graph)
    
    # Run the strategy
    strategy.run()
    
    # Save features and check if the file exists
    strategy.save_features("test_output.txt")
    with open("test_output.txt", "r") as f:
        content = f.read()
    assert len(content) > 0  # Ensure the file has content
