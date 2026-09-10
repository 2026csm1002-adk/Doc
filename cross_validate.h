#ifndef CROSS_VALIDATE_H
#define CROSS_VALIDATE_H

#include "csr.h"

struct NegativeCycleValidationResult {
    bool bfSays = false;
    bool fwSays = false;
    bool groundTruthAnyReachable = false;
    int firstGroundTruthSourceWithCycle = -1;
    bool consistent = false;
};

NegativeCycleValidationResult crossValidateNegativeCycle(const CSRGraph& csr, int source);

#endif