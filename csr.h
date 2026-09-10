#ifndef CSR_H
#define CSR_H

#include <vector>
#include <string>
using namespace std;

struct EdgeListEntry {
    int u, v;
    double w;
};

struct EdgeListGraph {
    int V = 0;
    long long E = 0;
    bool directed = true;
    vector<EdgeListEntry> edges; // preserved in input order
};

struct CSRGraph {
    int V = 0;
    long long E = 0; // number of directed edges stored (= sum of degrees written in file)
    vector<long long> row_ptr;
    vector<int> col_idx;
    vector<double> values; // empty if unweighted
    bool weighted = false;
};

struct AdjEntry {
    int neighbor;
    double weight;
};

struct AdjacencyList{
    int V = 0;
    long long E = 0; // number of directed edges stored
    int source = 0;
    bool weighted = false;
    vector<vector<AdjEntry>> adj; // Each Adjacency entry will store the list of AdjEntry

    // Optional Parameters for PageRank Algorithm

    double damping = 0.85;
    double tolerance = 1e-6;
    int maxIterations = 1000;
};

EdgeListGraph readEdgeList(const string& path);
CSRGraph edgeListToCSR(const EdgeListGraph& elg);
// Builds the transpose of a CSR graph (reverses every edge direction)
// directly via counting sort — no adjacency list or matrix intermediate.
CSRGraph transposeCSR(const CSRGraph& csr);
AdjacencyList readAdjacencyList(const string& path, bool weighted);

CSRGraph adjacencyListToCSR(const AdjacencyList& adjList);
#endif
