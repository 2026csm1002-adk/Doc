#include "csr.h"
#include <bits/stdc++.h>

// Add these to your existing csr.cpp, after the current adjacency-list code:
AdjacencyList readAdjacencyList(const string& path, bool weighted) {

    // Error Handling while reading the file
    ifstream in(path);
    if (!in.is_open()) {
        throw runtime_error("Could not open input file: " + path);
    }

    AdjacencyList result;
    result.weighted = weighted;

    long long V, E;
    if (!(in >> V >> E)) {
        throw runtime_error("Malformed header (expected: V E) in file: " + path);
    }
    result.V = (int)V;
    result.E = E;
    result.adj.assign(result.V, {});

    for (int line = 0; line < result.V; ++line) {
        int u, degree;
        if (!(in >> u >> degree)) {
            throw runtime_error("Malformed adjacency line in file: " + path);
        }
        if (u < 0 || u >= result.V) {
            throw runtime_error("Vertex id out of range in file: " + path);
        }
        result.adj[u].reserve(degree);
        for (int d = 0; d < degree; ++d) {
            int nbr;
            if (!(in >> nbr)) {
                throw runtime_error("Malformed neighbor list in file: " + path);
            }
            double w = 1.0;
            if (weighted) {
                if (!(in >> w)) {
                    throw runtime_error("Expected edge weight in file: " + path);
                }
                // if (w <= 0.0) {
                //     throw runtime_error("Edge weights must be positive in file: " + path);
                // } Removed due to assignment 2
            }
            result.adj[u].push_back({nbr, w});
        }
    }

    string tok;
    if (in >> tok) {
        if (tok == "SOURCE") {
            in >> result.source;
        }
        else if(tok == "DAMPING"){
            in >> result.damping;
        }
        else if(tok == "TOLERANCE"){
            in >> result.tolerance;
        }
        else if(tok == "MAX_ITERATIONS"){
            in >> result.maxIterations;
        }
    }

    return result;
}
EdgeListGraph readEdgeList(const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) {
        throw std::runtime_error("Could not open input file: " + path);
    }

    EdgeListGraph g;

    char typeChar;
    if (!(in >> typeChar)) {
        throw std::runtime_error("Malformed edge-list file (missing D/U header): " + path);
    }
    if (typeChar == 'D' || typeChar == 'd') {
        g.directed = true;
    } else if (typeChar == 'U' || typeChar == 'u') {
        g.directed = false;
    } else {
        throw std::runtime_error("Invalid graph type character (expected D or U): " + path);
    }

    int u, v;
    double w;
    int maxVertex = -1;
    while (in >> u >> v >> w) {
        if (u < 0 || v < 0) {
            throw std::runtime_error("Negative vertex id encountered in file: " + path);
        }
        g.edges.push_back({u, v, w});
        maxVertex = std::max(maxVertex, std::max(u, v));
    }

    g.V = maxVertex + 1; // V = max_vertex_id + 1
    g.E = (long long)g.edges.size();
    return g;
}

CSRGraph edgeListToCSR(const EdgeListGraph& elg) {
    CSRGraph csr;
    csr.V = elg.V;
    csr.weighted = true;

    // Pass 1: count each vertex's CSR entry count. No adjacency list or
    // matrix is built anywhere in this function.
    std::vector<long long> degree(csr.V, 0);
    for (const auto& e : elg.edges) {
        degree[e.u]++;
        if (!elg.directed && e.u != e.v) {
            degree[e.v]++; // undirected non-self-loop edge appears at both endpoints
        }
        // undirected self-loop (u == v): counted exactly once above
    }

    csr.row_ptr.assign(csr.V + 1, 0);
    for (int i = 0; i < csr.V; ++i) {
        csr.row_ptr[i + 1] = csr.row_ptr[i] + degree[i];
    }
    csr.E = csr.row_ptr[csr.V];

    csr.col_idx.assign(csr.E, 0);
    csr.values.assign(csr.E, 0.0);

    // Pass 2: fill entries using a per-vertex write cursor derived from row_ptr.
    std::vector<long long> cursor(csr.row_ptr.begin(), csr.row_ptr.end() - 1);

    for (const auto& e : elg.edges) {
        csr.col_idx[cursor[e.u]] = e.v;
        csr.values[cursor[e.u]] = e.w;
        cursor[e.u]++;

        if (!elg.directed && e.u != e.v) {
            csr.col_idx[cursor[e.v]] = e.u;
            csr.values[cursor[e.v]] = e.w;
            cursor[e.v]++;
        }
    }

    return csr;
}

CSRGraph transposeCSR(const CSRGraph& csr) {
    CSRGraph t;
    t.V = csr.V;
    t.weighted = csr.weighted;

    // Pass 1: count in-degree of every vertex.
    std::vector<long long> indeg(csr.V, 0);
    for (int u = 0; u < csr.V; ++u)
        for (long long e = csr.row_ptr[u]; e < csr.row_ptr[u + 1]; ++e)
            indeg[csr.col_idx[e]]++;

    t.row_ptr.assign(csr.V + 1, 0);
    for (int i = 0; i < csr.V; ++i) t.row_ptr[i + 1] = t.row_ptr[i] + indeg[i];
    t.E = t.row_ptr[csr.V];

    t.col_idx.assign(t.E, 0);
    if (t.weighted) t.values.assign(t.E, 0.0);

    // Pass 2: fill using a per-vertex write cursor.
    std::vector<long long> cursor(t.row_ptr.begin(), t.row_ptr.end() - 1);
    for (int u = 0; u < csr.V; ++u) {
        for (long long e = csr.row_ptr[u]; e < csr.row_ptr[u + 1]; ++e) {
            int v = csr.col_idx[e];
            t.col_idx[cursor[v]] = u;
            if (t.weighted) t.values[cursor[v]] = csr.values[e];
            cursor[v]++;
        }
    }
    return t;
}

CSRGraph adjacencyListToCSR(const AdjacencyList& adjList) {
    CSRGraph csr;
    csr.V = adjList.V;
    csr.weighted = adjList.weighted;

    csr.row_ptr.assign(csr.V + 1, 0);
    for (int u = 0; u < csr.V; ++u) {
        csr.row_ptr[u + 1] = csr.row_ptr[u] + (long long)adjList.adj[u].size();
    }
    csr.E = csr.row_ptr[csr.V];

    csr.col_idx.assign(csr.E, 0);
    if (csr.weighted) csr.values.assign(csr.E, 0.0);

    for (int u = 0; u < csr.V; ++u) {
        long long base = csr.row_ptr[u];
        const auto& list = adjList.adj[u];
        for (size_t k = 0; k < list.size(); ++k) {
            csr.col_idx[base + k] = list[k].neighbor;
            if (csr.weighted) csr.values[base + k] = list[k].weight;
        }
    }

    return csr;
}