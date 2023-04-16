#include "liveness_analysis.h"
#include "linear_order.h"

void LivenessAnalysis::RunPassImpl(Graph* g)
{
    g->RunPass<LinearOrder>();
    linear_order_ = g->GetLinearOrder();
    
    InitLiveness();
    CalculateLifeRanges(g);
}

void LivenessAnalysis::InitLiveness()
{
    uint32_t cur_live_num = 0;
    uint32_t cur_lin_num = 0;
    for (auto bb: linear_order_) {
        uint32_t bb_live_range_start = cur_live_num;
        for (auto inst = bb->GetFirstInst(); inst != nullptr; inst = inst->GetNext()) {
            if (inst->GetType() != Type::InstPhi) {
                cur_live_num += 2;
            }
            inst->SetLiveNumber(cur_live_num);
            inst->SetLinearNumber(cur_lin_num);
            cur_lin_num++;
        }
        cur_live_num += 2;
        bb->SetLiveInterval({bb_live_range_start, cur_live_num});
    }
}

void LivenessAnalysis::CalculateLifeRanges(Graph *g)
{
    for (auto bb = linear_order_.rbegin(); bb != linear_order_.rend(); bb++) {
        LiveInterval live_interval;
        for (auto succ: (*bb)->GetSuccs()) {
            live_interval.Union(succ->GetLiveInterval());
        }
    }
}