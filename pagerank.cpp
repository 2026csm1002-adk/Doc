#include "pagerank.h"
PageRankResult blockedPageRank(const CSRGraph& csr, double damping, double tolerance, int maxIterations, int B) {
    int N = csr.V;
    std::vector<double> rank(N, 1.0 / N);
    std::vector<long long> outdeg(N);
    for (int u = 0; u < N; ++u) outdeg[u] = csr.row_ptr[u + 1] - csr.row_ptr[u];

    PageRankResult result;
    for (int it = 1; it <= maxIterations; ++it) {
        double danglingSum = 0.0;
        for (int u = 0; u < N; ++u) if (outdeg[u] == 0) danglingSum += rank[u];
        double base = (1.0 - damping) / N + damping * danglingSum / N;
        std::vector<double> newRank(N, base);

        // Process source vertices in blocks of B instead of one sweep 0..N —
        // keeps each block's row_ptr/col_idx/values slice hot in cache
        // before moving to the next block.
        for (int blockStart = 0; blockStart < N; blockStart += B) {
            int blockEnd = std::min(N, blockStart + B);
            for (int u = blockStart; u < blockEnd; ++u) {
                if (outdeg[u] == 0) continue;
                double contribution = damping * rank[u] / (double)outdeg[u];
                for (long long e = csr.row_ptr[u]; e < csr.row_ptr[u + 1]; ++e) {
                    newRank[csr.col_idx[e]] += contribution;
                }
            }
        }

        double totalChange = 0.0;
        for (int v = 0; v < N; ++v) totalChange += std::abs(newRank[v] - rank[v]);
        rank = std::move(newRank);
        result.iterations = it;
        if (totalChange <= tolerance) { result.converged = true; break; }
    }
    result.ranks = std::move(rank);
    return result;
}

PageRankResult weightedPageRank(const CSRGraph& csr, double damping, double tolerance,
                                  int maxIterations, const std::vector<double>& personalization) {
    int N = csr.V;
    std::vector<double> rank = personalization; // instead of uniform 1/N
    std::vector<double> outWeightSum(N, 0.0);
    for (int u = 0; u < N; ++u)
        for (long long e = csr.row_ptr[u]; e < csr.row_ptr[u+1]; ++e)
            outWeightSum[u] += csr.values[e]; // edge weight instead of uniform 1/outdeg

    PageRankResult result;
    for (int it = 1; it <= maxIterations; ++it) {
        double danglingSum = 0.0;
        for (int u = 0; u < N; ++u) if (outWeightSum[u] == 0.0) danglingSum += rank[u];

        std::vector<double> newRank(N);
        for (int v = 0; v < N; ++v)
            newRank[v] = (1.0 - damping) * personalization[v] + damping * danglingSum * personalization[v];

        for (int u = 0; u < N; ++u) {
            if (outWeightSum[u] == 0.0) continue;
            for (long long e = csr.row_ptr[u]; e < csr.row_ptr[u+1]; ++e) {
                double share = csr.values[e] / outWeightSum[u]; // weighted share, not 1/outdeg
                newRank[csr.col_idx[e]] += damping * rank[u] * share;
            }
        }
        double change = 0.0;
        for (int v = 0; v < N; ++v) change += std::abs(newRank[v]-rank[v]);
        rank = std::move(newRank);
        result.iterations = it;
        if (change <= tolerance) { result.converged = true; break; }
    }
    result.ranks = std::move(rank);
    return result;
}