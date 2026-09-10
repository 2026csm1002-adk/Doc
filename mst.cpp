#include "mst.h"
#include <algorithm>
#include <climits>
#include <numeric>
#include <queue>

using namespace std;

struct DSU{
    vector<int>parent;
    vector<int>rank;

    DSU(int n){
        parent.resize(n);
        rank.resize(n, 0);

        for(int i = 0; i < n; ++i){
            parent[i] = i;
        }
    }

    int findParent(int x){
        if(parent[x] != x){
            parent[x] = findParent(parent[x]);
        }
        return parent[x];
    }

    // returns true if they are disconnected
    // and false if they are connected
    bool unite(int a, int b){
        int rootA = findParent(a);
        int rootB = findParent(b);

        if(rootA == rootB) return false;

        if(rootA != rootB){
            if(rank[rootA] < rank[rootB]) parent[rootA] = rootB;
            else if(rank[rootA] > rank[rootB]) parent[rootB] = rootA;
            else{
                parent[rootB] = rootA;
                rank[rootA]++;
            } 
        }
        return true;
        
    }
};

MSTResult kruskalMST(const CSRGraph & csr){
    int V = csr.V;
    vector<MSTEdge>edges;

    for(int u = 0; u < V; u++){
        for(long long i = csr.row_ptr[u]; i < csr.row_ptr[u + 1]; ++i){
            int v = csr.col_idx[i];
            long long weight = csr.values[i];

            if(u < v) edges.push_back({u, v, weight});
        }
    }

    sort(edges.begin(), edges.end(), [](MSTEdge a, MSTEdge b){
        return a.weight < b.weight;
    });

    DSU dsu(V);
    MSTResult result;

    for(const auto &e: edges){
        if((int) result.edges.size() == V -1) break;
        if(dsu.unite(e.u, e.v)){
            result.edges.push_back(e);
            result.totalWeight += e.weight;
        }
    }
    int num_trees = 0;
    for(int i = 0; i < V; ++i){
        if(dsu.findParent(i) == i){
            num_trees++;
        }
    }
    result.num_components = num_trees;
    return result;
}

MSTResult primMST(const CSRGraph & csr, int startVertex){
    int V = csr.V;
    vector<bool>inMST(V, false);
    vector<long long>minEdgeWeight(V, LLONG_MAX);
    vector<int>parent(V, -1);

    using WV = pair<long long, int>;    // {weight, vertex}

    priority_queue<WV, vector<WV>, greater<WV>>pq;      // min-heap

    minEdgeWeight[startVertex] = 0;
    pq.push({0, startVertex});

    MSTResult result;

    while(!pq.empty()){
        auto [w, u] = pq.top();
        pq.pop();
        if(inMST[u]) continue;
        inMST[u] = true;

        if(parent[u] != -1){
            result.edges.push_back({parent[u], u, w});
            result.totalWeight += w;
        }

        for(long long e = csr.row_ptr[u]; e < csr.row_ptr[u+1]; ++e){
            int v = csr.col_idx[e];
            long long weight = (long long)csr.values[e];
            if(!inMST[v] && weight < minEdgeWeight[v]){
                minEdgeWeight[v] = weight;
                parent[v] = u;
                pq.push({weight, v});
            }
        }
    }
    
    return result;
}