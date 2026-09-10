#ifndef PAGERANK_H
#define PAGERANK_H

#include <vector>
#include "csr.h"
using namespace std;

struct PageRankResult {
    vector<double> ranks;
    int iterations = 0;
    bool converged = false;
};

PageRankResult blockedPageRank(const CSRGraph& csr, double damping, double tolerance, int maxIterations);
PageRankResult weightedPageRank(const CSRGraph& csr, double damping, double tolerance, int maxIterations, const std::vector<double>& personalization);
#endif