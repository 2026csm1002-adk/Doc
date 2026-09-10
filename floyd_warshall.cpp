#include "floyd_warshall.h"
vector<vector<long long>> csrToDistanceMatrix(const CSRGraph& csr) {
    int V = csr.V;
    std::vector<vector<long long>> dist(V, vector<long long>(V, FW_INF));
    for (int i = 0; i < V; ++i) dist[i][i] = 0;

    for (int u = 0; u < V; ++u) {
        for (long long e = csr.row_ptr[u]; e < csr.row_ptr[u + 1]; ++e) {
            int v = csr.col_idx[e];
            long long w = (long long)csr.values[e];
            if (u == v) {
                if (w < dist[u][u]) dist[u][u] = w;
                continue;
            }
            if (w < dist[u][v]) dist[u][v] = w; // take min across parallel edges
        }
    }
    return dist;
}


FloydWarshallResult floydWarshall(vector<vector<long long>> dist) {
    int V = (int)dist.size();

    for (int k = 0; k < V; ++k) {
        for (int i = 0; i < V; ++i) {
            if (dist[i][k] >= FW_INF) continue;

            for (int j = 0; j < V; ++j) {
                if (dist[k][j] >= FW_INF) continue;

                long long viaK = dist[i][k] + dist[k][j];

                if (viaK < dist[i][j]) {
                    dist[i][j] = viaK;
                }
            }
        }
    }

    FloydWarshallResult result;
    result.dist = std::move(dist);
    result.negativeCycle = false;

    for (int i = 0; i < V; ++i) {
        if (result.dist[i][i] < 0) {
            result.negativeCycle = true;
            break;
        }
    }

    return result;
}

FloydWarshallResult blockedFloydWarshall(vector<vector<long long>> dist, int B) {
    int V = (int)dist.size();
    if (B <= 0) B = 32;
    int numBlocks = (V + B - 1) / B;

    // Flat, row-major working array — required for blocking to have any
    // real cache benefit (see explanation above).
    std::vector<long long> flat((size_t)V * V);
    for (int i = 0; i < V; ++i)
        for (int j = 0; j < V; ++j)
            flat[(size_t)i * V + j] = dist[i][j];

    auto blockStart = [&](int b) { return b * B; };
    auto blockEnd = [&](int b) { return std::min(V, (b + 1) * B); };

    auto relax = [&](int iLo, int iHi, int kLo, int kHi, int jLo, int jHi) {
        for (int k = kLo; k < kHi; ++k) {
            for (int i = iLo; i < iHi; ++i) {
                long long dik = flat[(size_t)i * V + k];
                if (dik >= FW_INF) continue;
                long long* rowI = &flat[(size_t)i * V];
                const long long* rowK = &flat[(size_t)k * V];
                for (int j = jLo; j < jHi; ++j) {
                    long long dkj = rowK[j];
                    if (dkj >= FW_INF) continue;
                    long long viaK = dik + dkj;
                    if (viaK < rowI[j]) rowI[j] = viaK;
                }
            }
        }
    };

    for (int r = 0; r < numBlocks; ++r) {
        int kLo = blockStart(r), kHi = blockEnd(r);

        relax(kLo, kHi, kLo, kHi, kLo, kHi); // Phase 1: diagonal block

        for (int j = 0; j < numBlocks; ++j) { // Phase 2a: row blocks
            if (j == r) continue;
            int jLo = blockStart(j), jHi = blockEnd(j);
            relax(kLo, kHi, kLo, kHi, jLo, jHi);
        }

        for (int i = 0; i < numBlocks; ++i) { // Phase 2b: column blocks
            if (i == r) continue;
            int iLo = blockStart(i), iHi = blockEnd(i);
            relax(iLo, iHi, kLo, kHi, kLo, kHi);
        }

        for (int i = 0; i < numBlocks; ++i) { // Phase 3: remaining blocks
            if (i == r) continue;
            int iLo = blockStart(i), iHi = blockEnd(i);
            for (int j = 0; j < numBlocks; ++j) {
                if (j == r) continue;
                int jLo = blockStart(j), jHi = blockEnd(j);
                relax(iLo, iHi, kLo, kHi, jLo, jHi);
            }
        }
    }

    FloydWarshallResult result;
    result.dist.assign(V, vector<long long>(V));
    for (int i = 0; i < V; ++i)
        for (int j = 0; j < V; ++j)
            result.dist[i][j] = flat[(size_t)i * V + j];

    for (int i = 0; i < V; ++i) {
        if (result.dist[i][i] < 0) { result.negativeCycle = true; break; }
    }
    return result;
}

// Add to floyd_warshall.cpp:
static std::vector<std::vector<long long>> minPlusMultiply(
    const std::vector<std::vector<long long>>& A,
    const std::vector<std::vector<long long>>& B) {
    int V = (int)A.size();
    std::vector<std::vector<long long>> C(V, std::vector<long long>(V, FW_INF));
    for (int i = 0; i < V; ++i) {
        for (int k = 0; k < V; ++k) {
            if (A[i][k] >= FW_INF) continue;
            for (int j = 0; j < V; ++j) {
                if (B[k][j] >= FW_INF) continue;
                long long v = A[i][k] + B[k][j];
                if (v < C[i][j]) C[i][j] = v;
            }
        }
    }
    return C;
}

FloydWarshallResult floydWarshallViaGEMM(std::vector<std::vector<long long>> dist) {
    int V = (int)dist.size();
    int steps = 0;
    long long n = 1;
    while (n < V) { n *= 2; ++steps; } // steps = ceil(log2(V))
    for (int s = 0; s < steps; ++s) dist = minPlusMultiply(dist, dist);

    FloydWarshallResult result;
    result.dist = dist;
    for (int i = 0; i < V; ++i) if (dist[i][i] < 0) { result.negativeCycle = true; break; }
    return result;
}