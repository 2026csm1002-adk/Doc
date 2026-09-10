// New file: assignment_01/driver/driver_csr_edgelist.cpp

#include "csr.h"
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>\n";
        return 1;
    }
    std::string path = argv[1];

    EdgeListGraph elg;
    try {
        elg = readEdgeList(path);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    CSRGraph csr = edgeListToCSR(elg);

    std::cout << "Offset Array (row_ptr):\n";
    for (size_t i = 0; i < csr.row_ptr.size(); ++i) {
        std::cout << csr.row_ptr[i];
        if (i + 1 < csr.row_ptr.size()) std::cout << " ";
    }
    std::cout << "\n";

    std::cout << "Intermediate Array (col_idx):\n";
    for (size_t i = 0; i < csr.col_idx.size(); ++i) {
        std::cout << csr.col_idx[i];
        if (i + 1 < csr.col_idx.size()) std::cout << " ";
    }
    std::cout << "\n";

    std::cout << "Property Array (values):\n";
    for (size_t i = 0; i < csr.values.size(); ++i) {
        double v = csr.values[i];
        if (v == (long long)v) std::cout << (long long)v;
        else std::cout << v;
        if (i + 1 < csr.values.size()) std::cout << " ";
    }
    std::cout << "\n";

    return 0;
}