#ifndef LIVENESS_ANALYSIS_H
#define LIVENESS_ANALYSIS_H

#include <unordered_map>

#include "ir/graph.h"
#include "ir/liveness_info.h"

class LivenessAnalysis {
public:
    void RunPassImpl(Graph *g);

private:
    void InitLiveness();
    void CalculateLifeIntervals(Graph *g);
    void AddPhiInputsToLiveset(BasicBlock *curr_bb, BasicBlock *bb, LiveSet& live_set);
    void IterateOverInputs(Inst* inst, LiveSet& live_set);
    void AddInstLiveInterval(Inst* inst, uint32_t start, uint32_t end);

    std::vector<BasicBlock*> linear_order_;
    std::unordered_map<BasicBlock*, LiveSet> live_inputs_;
    std::unordered_map<BasicBlock*, LiveInterval> bb_live_interval_;
    std::unordered_map<Inst*, LiveInterval*> inst_live_interval_;
};

#endif // LIVENESS_ANALYSIS_H
