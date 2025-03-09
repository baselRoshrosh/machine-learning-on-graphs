import semProject as sp

import os
import zipfile
import shutil  # for deleting temporary files

# Strategies to perform
strategies = [sp.KNN, sp.Topo2Vec, sp.AttributedDeepwalk]
strategy_names = ['KNN', 'Topo2Vec', 'AttributedDeepwalk']

# Paths
REF_FOLDER = "./extlibs/eval/ref"
INPUT_FOLDER = "./input"
OUTPUT_FOLDER = "./output"
TEMP_UNZIP_FOLDER = "./temp_unzip"

if not os.path.exists(TEMP_UNZIP_FOLDER):
    os.makedirs(TEMP_UNZIP_FOLDER)  # Create directory if it doesn't exist 

# Get list of available graphs in ref (without extensions)
graph_names = [f.split('.')[0] for f in os.listdir(REF_FOLDER) if os.path.isfile(os.path.join(REF_FOLDER, f))]

# for each graph in "ref" References of eval script:
for graph_name in graph_names:
    print(f"Read graph: {graph_name}")
    
    # extracting graph files
    with zipfile.ZipFile(os.path.join(INPUT_FOLDER, graph_name + ".zip"), 'r') as zip_ref:
        zip_ref.extractall(TEMP_UNZIP_FOLDER)
        
    # performing strategies
    for i, strategy in enumerate(strategies):
        feature_file = os.path.join(TEMP_UNZIP_FOLDER, graph_name + "_features.txt")
        edge_file = os.path.join(TEMP_UNZIP_FOLDER, graph_name + "_edges.txt")
        print(feature_file)
        
        # 1: getting graph
        graph = sp.Graph(feature_file, edge_file)

        # 2: initializing strategies
        print(f"performing strategy {strategy_names[i]}")
        x = strategy(graph)
        
        # 3: configure
        
        # 4: run and extract
        x.run()
        results = x.extract_results()
        x.save_features(results, os.path.join(OUTPUT_FOLDER, f"{graph_name}_{strategy_names[i]}.txt"))
        
# Clean up by removing unzipped graph files
shutil.rmtree(TEMP_UNZIP_FOLDER)
os.makedirs(TEMP_UNZIP_FOLDER)  # rebuilding temporary dir