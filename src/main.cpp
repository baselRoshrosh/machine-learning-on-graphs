#include "iostream"

#include "StrategyRunner.hpp"
#include "KNN.hpp"

using namespace std;

int main()
{
    cout << "Reading twitch graph..." << endl;
    shared_ptr<Graph> rawGraph = make_shared<Graph>("../input/twitch/twitch_features.txt", "../input/twitch/twitch_edges.txt");

    cout << "Applying knn on genius graph..." << endl;
    StrategyRunner<KNN> knn = StrategyRunner<KNN>(rawGraph);

    map<string, double> params = {{"k", 5}, {"maxIterations", 2}};
    knn.configure(params);
    
    cout << "Running with k=5..." << endl;
    knn.run();
    
    auto resultGraph = knn.extractResults();
    cout << "Extracted result" << endl;
    
    cout << "Saving to file..." << endl;
    knn.saveFeatures(resultGraph, "../results/twitch_features_result.txt");
    
    knn.reset();
    cout << "Reset parameters for knn" << endl;

    return 0;
}