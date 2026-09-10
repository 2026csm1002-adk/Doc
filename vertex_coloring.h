#ifndef VERTEX_COLORING_H
#define VERTEX_COLORING_H

#include <vector>
#include "csr.h"
using namespace std;

struct VertexColoringResult{
    vector<int>color;
    int colorsUsed = 0;         // Number of colors
};

VertexColoringResult vertexColoring(const CSRGraph &csr);
bool isValidColoring(const CSRGraph &csr, const vector<int>&color);
bool hasSelfLoop(const CSRGraph& csr);
VertexColoringResult greedyVertexColoringOptimized(const CSRGraph& csr);

#endif