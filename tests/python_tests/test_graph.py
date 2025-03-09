import pytest
import semProject
from . import GRAPH_PATHS

def test_graph_node_count():
    # Initialize the Graph with mock files or actual test files
    graph = semProject.Graph(GRAPH_PATHS["nodes"], GRAPH_PATHS["edges"])
    
    # Check if getNodeCount works as expected
    node_count = graph.getNodeCount()
    assert node_count == 183

def test_graph_edge_count():
    # Initialize the Graph with mock files or actual test files
    graph = semProject.Graph("test_nodes.txt", "test_edges.txt")
    
    # Check if getEdgeCount works as expected
    edge_count = graph.getEdgeCount()
    assert edge_count == 298
