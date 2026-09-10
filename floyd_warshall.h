#ifndef FLOYD_WARSHALL_H
#define FLOYD_WARSHALL_H

#include <vector>
#include "csr.h"
using namespace std;

constexpr long long FW_INF = 1LL << 50;

struct FloydWarshallResult{
    vector<vector<long long>>dist;
    bool negativeCycle = false;
};

FloydWarshallResult floydWarshall(vector<vector<long long>>dist);
vector<vector<long long>> csrToDistanceMatrix(const CSRGraph& csr);
FloydWarshallResult blockedFloydWarshall(vector<vector<long long>> dist, int B);

FloydWarshallResult floydWarshallViaGEMM(std::vector<std::vector<long long>> dist);
#endif