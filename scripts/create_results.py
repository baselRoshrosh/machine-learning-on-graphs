import semProject as sp
import os
import zipfile
import shutil
import argparse

# Paths
PROJECT_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
REF_FOLDER = os.path.join(PROJECT_ROOT, "extlibs/eval/ref")
INPUT_FOLDER = os.path.join(PROJECT_ROOT, "input")
OUTPUT_FOLDER = os.path.join(PROJECT_ROOT, "output")
TEMP_UNZIP_FOLDER = os.path.join(PROJECT_ROOT, "temp_unzip")

# Ensure necessary directories exist
os.makedirs(TEMP_UNZIP_FOLDER, exist_ok=True)
os.makedirs(OUTPUT_FOLDER, exist_ok=True)

# Mapping strategy names to classes
strategy_map = {
    "KNN": sp.KNN,
    "Topo2Vec": sp.Topo2Vec,
    "AttributedDeepwalk": sp.AttributedDeepwalk
}

def main(graph_name, strategy_name):
    if strategy_name not in strategy_map:
        print(f"Error: Unknown strategy '{strategy_name}'. Available options: {', '.join(strategy_map.keys())}")
        return
    
    graph_path = os.path.join(INPUT_FOLDER, f"{graph_name}.zip")
    if not os.path.exists(graph_path):
        print(f"Error: Graph '{graph_name}' not found in '{INPUT_FOLDER}'.")
        return


    print(f"Unpacking graph: {graph_name}")
    # Extract graph files
    with zipfile.ZipFile(graph_path, 'r') as zip_ref:
        zip_ref.extractall(TEMP_UNZIP_FOLDER)
    
    
    print(f"Processing with strategy: {strategy_name}")
    feature_file = os.path.join(TEMP_UNZIP_FOLDER, f"{graph_name}_features.txt")
    edge_file = os.path.join(TEMP_UNZIP_FOLDER, f"{graph_name}_edges.txt")
    
    if not os.path.exists(feature_file) or not os.path.exists(edge_file):
        print(f"Error: Missing extracted files for graph '{graph_name}'.")
        return

    # Initialize graph and strategy
    graph = sp.Graph(feature_file, edge_file)
    strategy = strategy_map[strategy_name](graph)
    
    # Run strategy
    strategy.run()
    
    # Save results
    output_dir = os.path.join(OUTPUT_FOLDER, f"{graph_name}_{strategy_name}")
    os.makedirs(output_dir, exist_ok=True)  # Create the output directory if it doesn't exist

    output_path = os.path.join(output_dir, f"{graph_name}_features.txt")
    strategy.save_features(graph, output_path)
        
    # Create the zip file in the desired folder
    zip_filename = os.path.join(output_dir, f"{graph_name}.zip")
    with zipfile.ZipFile(zip_filename, 'w', zipfile.ZIP_DEFLATED) as zipf:
        # Add the output file inside the zip with a new name (graph_strategy.txt)
        zipf.write(output_path, f"{graph_name}_features.txt")
    
    print(f"Results zipped to: {zip_filename}")
    
    # Optionally, delete the original unzipped file after zipping
    os.remove(output_path)

    # Cleanup temporary files
    print("clean up")
    shutil.rmtree(TEMP_UNZIP_FOLDER)
    os.makedirs(TEMP_UNZIP_FOLDER, exist_ok=True)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Run a specific strategy on a given graph.")
    parser.add_argument("graph", type=str, help="The name of the graph (without file extension)")
    parser.add_argument("strategy", type=str, help="The name of the strategy to run (KNN, Topo2Vec, AttributedDeepwalk)")
    args = parser.parse_args()
    
    main(args.graph, args.strategy)
