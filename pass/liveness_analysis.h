#ifndef LIVENESS_ANALYSIS_H
#define LIVENESS_ANALYSIS_H

#include <unordered_map>

#include "ir/graph.h"
#include "ir/live_interval.h"

class LivenessAnalysis {
public:
    void RunPassImpl(Graph *g);

private:
    void InitLiveness();
    void CalculateLifeRanges(Graph *g);

    std::vector<BasicBlock*> linear_order_;
    std::unordered_map<uint32_t, LiveInterval> live_intervals_;
};

#endif // LIVENESS_ANALYSIS_H
