#include "bellman_ford.h"

BellmanFordResult bellmanFord(const CSRGraph &csr, int src){
    int V = csr.V;
    BellmanFordResult result;
    result.dist.assign(V, BF_INF);
    result.dist[src] = 0;
    
    for(int iter = 0; iter < V - 1; ++iter){
        for(int u = 0; u < V; ++u){
            if(result.dist[u] >= BF_INF) continue;
            for(long long e = csr.row_ptr[u]; e < csr.row_ptr[u+1]; ++e){
                int v = csr.col_idx[e];
                long long w = (long long)csr.values[e];
                if(result.dist[u]  + w < result.dist[v]){
                    result.dist[v] = result.dist[u] + w;
                }
            }
        }
    }


    for(int u = 0; u < V; ++u){
        if(result.dist[u] >= BF_INF) continue;
        for(long long e = csr.row_ptr[u]; e < csr.row_ptr[u+1]; ++e){
            int v = csr.col_idx[e];
            long long w = (long long)csr.values[e];
            if(result.dist[u] + w < result.dist[v]){
                result.negativeCycle = true;
                return result;
            }
        }
    }
    return result;
}