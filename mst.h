#ifndef MST_H
#define MST_H

#include <vector>
#include "csr.h"
using namespace std;
struct MSTEdge{
    int u, v;
    long long weight;
};

struct MSTResult{
    vector<MSTEdge>edges;
    long long totalWeight = 0;
    int num_components = 0;
};

MSTResult kruskalMST(const CSRGraph& csr);

MSTResult primMST(const CSRGraph& csr, int startVertex = 0);

#endif