#include "vertex_coloring.h"
#include <algorithm>
#include <numeric>
using namespace std;

VertexColoringResult vertexColoring(const CSRGraph& csr) {
    int V = csr.V;
    VertexColoringResult result;
    result.color.assign(V, -1);

    vector<int> order(V);
    iota(order.begin(), order.end(), 0);
    sort(order.begin(), order.end(), [&](int a, int b) {
        long long degA = csr.row_ptr[a + 1] - csr.row_ptr[a];
        long long degB = csr.row_ptr[b + 1] - csr.row_ptr[b];
        return degA > degB;
    });

    int maxColorSeen = -1;
    vector<bool> usedByNeighbour;

    for (int u : order) {
        usedByNeighbour.assign(maxColorSeen + 2, false);
        for (long long e = csr.row_ptr[u]; e < csr.row_ptr[u + 1]; ++e) {
            int nbr = csr.col_idx[e];
            int c = result.color[nbr];
            if (c >= 0 && c < (int)usedByNeighbour.size()) usedByNeighbour[c] = true;
        }

        int chosen = 0;
        while (chosen < (int)usedByNeighbour.size() && usedByNeighbour[chosen]) ++chosen;

        result.color[u] = chosen;
        if (chosen > maxColorSeen) maxColorSeen = chosen;
    }

    result.colorsUsed = maxColorSeen + 1;
    return result;
}

bool isValidColoring(const CSRGraph& csr, const vector<int>& color) {
    for (int u = 0; u < csr.V; ++u)
        for (long long e = csr.row_ptr[u]; e < csr.row_ptr[u + 1]; ++e)
            if (color[u] == color[csr.col_idx[e]]) return false;
    return true;
}

bool hasSelfLoop(const CSRGraph& csr) {
    for (int u = 0; u < csr.V; ++u)
        for (long long e = csr.row_ptr[u]; e < csr.row_ptr[u + 1]; ++e)
            if (csr.col_idx[e] == u) return true;
    return false;
}

VertexColoringResult greedyVertexColoringOptimized(const CSRGraph& csr) {
    int V = csr.V;
    VertexColoringResult result;
    result.color.assign(V, -1);

    std::vector<int> order(V);
    std::iota(order.begin(), order.end(), 0);
    std::sort(order.begin(), order.end(), [&](int a, int b) {
        return (csr.row_ptr[a+1]-csr.row_ptr[a]) > (csr.row_ptr[b+1]-csr.row_ptr[b]);
    });

    // Instead of re-zeroing a "usedByNeighbour" array every vertex (O(colors)
    // work per vertex even when few neighbors are colored), track which
    // color each neighbor uses directly, using a "last seen at vertex u"
    // timestamp array to avoid needing to reset anything between vertices.
    std::vector<int> lastMarked(V + 1, -1); // lastMarked[color] = which vertex "used" it most recently
    int maxColorSeen = -1;

    for (int u : order) {
        for (long long e = csr.row_ptr[u]; e < csr.row_ptr[u + 1]; ++e) {
            int c = result.color[csr.col_idx[e]];
            if (c >= 0) lastMarked[c] = u; // mark color c as "forbidden for u"
        }
        int chosen = 0;
        while (chosen <= maxColorSeen + 1 && lastMarked[chosen] == u) ++chosen;
        result.color[u] = chosen;
        if (chosen > maxColorSeen) maxColorSeen = chosen;
    }
    result.colorsUsed = maxColorSeen + 1;
    return result;
}