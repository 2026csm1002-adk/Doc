#include "csr.h"
#include "floyd_warshall.h"
#include <chrono>
#include <iostream>
#include <string>
#include <vector>

static bool matricesEqual(const FloydWarshallResult& a, const FloydWarshallResult& b, int V) {
    if (a.negativeCycle != b.negativeCycle) return false;
    if (a.negativeCycle) return true;
    for (int i = 0; i < V; ++i)
        for (int j = 0; j < V; ++j)
            if (a.dist[i][j] != b.dist[i][j]) return false;
    return true;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file> [B1 B2 B3 ...]\n";
        return 1;
    }
    std::string path = argv[1];

    std::vector<int> blockSizes;
    for (int i = 2; i < argc; ++i) blockSizes.push_back(std::stoi(argv[i]));
    if (blockSizes.empty()) blockSizes = {16, 32, 64};

    EdgeListGraph elg;
    try {
        elg = readEdgeList(path);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    CSRGraph csr = edgeListToCSR(elg);
    auto distInit = csrToDistanceMatrix(csr); // NOT timed
    int V = csr.V;

    auto tN1 = std::chrono::high_resolution_clock::now();
    FloydWarshallResult naive = floydWarshall(distInit);
    auto tN2 = std::chrono::high_resolution_clock::now();
    double naiveMs = std::chrono::duration<double, std::milli>(tN2 - tN1).count();

    std::cout << "V: " << V << "\n";
    std::cout << "Naive Floyd-Warshall runtime: " << naiveMs << " ms\n\n";

    std::cout << "| V | Block size | Runtime (ms) | Correctness |\n";
    std::cout << "|---|---|---|---|\n";

    for (int B : blockSizes) {
        auto t1 = std::chrono::high_resolution_clock::now();
        FloydWarshallResult blocked = blockedFloydWarshall(distInit, B);
        auto t2 = std::chrono::high_resolution_clock::now();
        double ms = std::chrono::duration<double, std::milli>(t2 - t1).count();
        bool pass = matricesEqual(naive, blocked, V);
        std::cout << "| " << V << " | " << B << " | " << ms << " | " << (pass ? "PASS" : "FAIL") << " |\n";
    }
    return 0;
}