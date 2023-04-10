#include <queue>

#include "liveness_analysis.h"
#include "loop_analyzer.h"
#include "rpo.h"

void LivenessAnalysis::RunPassImpl(Graph* g)
{
    g->RunPass<RPO>();
    g->RunPass<LoopAnalyzer>();
    BuildLinearOrder(g);

    InitLiveness();
    CalculateLifeRanges(g);
}

// not sure that this algorithm is correct
void LivenessAnalysis::BuildLinearOrder(Graph* g)
{
    auto visit_marker = g->NewMarker();
    for (auto bb: g->GetRPOBasicBlocks()) {
        if (!bb->IsMarked(visit_marker) && bb->IsLoopHeader()) {
            ProccessLoop(bb->GetLoop(), visit_marker);
        } else {
            if (!bb->IsMarked(visit_marker)) {
                linear_order_.push_back(bb);
                bb->SetMarker(visit_marker);
            }
        }
    }

    g->SetLinearOrder(linear_order_);
}

void LivenessAnalysis::ProccessLoop(Loop* loop, marker visit_marker)
{
    // reversing, since blocks in loop are stored in reversed order
    for (int i = loop->GetBlocks().size() - 1; i >= 0; --i) {
        auto bb_loop = loop->GetBlocks()[i];
        if (bb_loop->IsLoopHeader() && bb_loop->GetLoop() != loop) {
            ProccessLoop(bb_loop->GetLoop(), visit_marker);
        // second condition is necessary to correctly process bbs from inner loops
        // TODO maybe a little change in loop analysis' logic?
        } else if (!bb_loop->IsMarked(visit_marker) && bb_loop->GetLoop() == loop) {
            linear_order_.push_back(bb_loop);
            bb_loop->SetMarker(visit_marker);
        }
    }
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