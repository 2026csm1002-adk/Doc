#include "cross_validate.h"
#include "bellman_ford.h"
#include "floyd_warshall.h"

NegativeCycleValidationResult crossValidateNegativeCycle(const CSRGraph& csr, int source) {
    NegativeCycleValidationResult result;

    result.bfSays = bellmanFord(csr, source).negativeCycle;
    result.fwSays = floydWarshall(csrToDistanceMatrix(csr)).negativeCycle;

    result.groundTruthAnyReachable = false;
    for (int s = 0; s < csr.V; ++s) {
        if (bellmanFord(csr, s).negativeCycle) {
            result.groundTruthAnyReachable = true;
            result.firstGroundTruthSourceWithCycle = s;
            break;
        }
    }

    bool fwMatchesGroundTruth = (result.fwSays == result.groundTruthAnyReachable);
    bool bfIsPlausible = (!result.bfSays) || result.groundTruthAnyReachable;

    result.consistent = fwMatchesGroundTruth && bfIsPlausible;
    return result;
}