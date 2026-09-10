#include "csr.h"
#include "cross_validate.h"
#include <chrono>
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>\n";
        return 1;
    }
    std::string path = argv[1];

    AdjacencyList adjList;
    try {
        adjList = readAdjacencyList(path, /*weighted=*/true);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    CSRGraph csr = adjacencyListToCSR(adjList);
    int source = adjList.source;

    auto t1 = std::chrono::high_resolution_clock::now();
    NegativeCycleValidationResult result = crossValidateNegativeCycle(csr, source);
    auto t2 = std::chrono::high_resolution_clock::now();
    double ms = std::chrono::duration<double, std::milli>(t2 - t1).count();

    std::cout << "=== Negative Cycle Cross-Validation ===\n";
    std::cout << "Source: " << source << "\n";
    std::cout << "Bellman-Ford (from source " << source << "): "
              << (result.bfSays ? "negative cycle detected" : "no negative cycle") << "\n";
    std::cout << "Floyd-Warshall (global check):           "
              << (result.fwSays ? "negative cycle detected" : "no negative cycle") << "\n";
    std::cout << "Ground truth (Bellman-Ford from every vertex): "
              << (result.groundTruthAnyReachable ? "negative cycle exists somewhere" : "no negative cycle anywhere")
              << "\n";
    if (result.groundTruthAnyReachable) {
        std::cout << "  (first detected when starting from vertex "
                  << result.firstGroundTruthSourceWithCycle << ")\n";
    }
    std::cout << "Consistent: " << (result.consistent ? "PASS" : "FAIL") << "\n";
    std::cout << "Execution time: " << ms << " ms\n";

    return 0;
}