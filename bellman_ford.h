#ifndef BELLMAN_FORD_H
#define BELLMAN_FORD_H

#include <vector>
#include "csr.h"

constexpr long long BF_INF = 1LL << 50;

struct BellmanFordResult {
    std::vector<long long> dist;
    bool negativeCycle = false;
};

BellmanFordResult bellmanFord(const CSRGraph &csr, int src);
#endif