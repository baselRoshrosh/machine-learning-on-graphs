#include "iostream"

#include "StrategyRunner.hpp"
#include "KNN.hpp"

int main()
{
    std::cout << "Reading twitch graph..." << std::endl;
    std::shared_ptr<Graph> rawGraph = std::make_shared<Graph>("../input/twitch/twitch_features.txt", "../input/twitch/twitch_edges.txt");

    std::cout << "Applying knn on genius graph..." << std::endl;
    StrategyRunner<KNN> knn = StrategyRunner<KNN>(rawGraph);

    std::map<std::string, double> params = {{"k", 5}, {"maxIterations", 2}};
    knn.configure(params);
    
    std::cout << "Running with k=5..." << std::endl;
    knn.run();
    
    auto resultGraph = knn.extractResults();
    std::cout << "Extracted result" << std::endl;
    
    std::cout << "Saving to file..." << std::endl;
    knn.saveFeatures(resultGraph, "../results/twitch_features_result.txt");
    
    knn.reset();
    std::cout << "Reset parameters for knn" << std::endl;

    return 0;
}