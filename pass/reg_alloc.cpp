#include "reg_alloc.h"
#include "liveness_analysis.h"

void RegAlloc::RunPassImpl(Graph* g)
{
    g->RunPass<LivenessAnalysis>();

    PrepareIntervals(g);

    LinearScan();
}

void RegAlloc::PrepareIntervals(Graph* g)
{
    std::vector<Inst*> to_erase;
    for (auto item: g->GetLiveIntervals()) {
        if (item.second->GetStart() == item.second->GetEnd()) {
            to_erase.push_back(item.first);
        } else {
            live_intervals_.push_back(item.second);
        }
    }

    // remove zero intervals
    for (auto item: to_erase) {
        g->GetLiveIntervals().erase(item);
    }

    auto comparator = [](LiveInterval* lhs, LiveInterval* rhs) { return lhs->GetStart() < rhs->GetStart(); };
    std::sort(live_intervals_.begin(), live_intervals_.end(), comparator);
}

void RegAlloc::LinearScan()
{
    for (auto interval: live_intervals_) {
        ExpireOldIntervals(interval);
        if (active_live_intervals_.size() == reg_num_) {
            SpillAtInterval(interval);
        } else {
            interval->SetLocation(ReserveReg());
            active_live_intervals_.push_back(interval);
        }
    }
}

void RegAlloc::ExpireOldIntervals(LiveInterval* cur_interval)
{
    std::vector<LiveInterval*> to_erase;

    SortActiveIntervals();

    for (auto active_interval: active_live_intervals_) {
        if (active_interval->GetEnd() > cur_interval->GetStart()) {
            break;
        }
        to_erase.push_back(active_interval);
        FreeReg(active_interval->GetLocation());
    }

    for (auto interval_to_erase: to_erase) {
        EraseElementFromVector(active_live_intervals_, interval_to_erase);
    }
}

void RegAlloc::SpillAtInterval(LiveInterval* cur_interval)
{
    LiveInterval * spill = nullptr;
    if (active_live_intervals_.size() > 0) {
        SortActiveIntervals();
        spill = active_live_intervals_.back();
    }

    if (active_live_intervals_.size() > 0 && spill->GetEnd() > cur_interval->GetEnd()) {
        cur_interval->SetLocation(spill->GetLocation());
        spill->SetLocation(cur_free_stack_slot_);
        spill->SetIsStackLocation(true);

        EraseElementFromVector(active_live_intervals_, spill);

        active_live_intervals_.push_back(cur_interval);
    } else {
        cur_interval->SetLocation(cur_free_stack_slot_);
        cur_interval->SetIsStackLocation(true);
    }
    ++cur_free_stack_slot_;
}

void RegAlloc::SortActiveIntervals()
{
    auto comparator = [](LiveInterval* lhs, LiveInterval* rhs){ return lhs->GetEnd() < rhs->GetEnd(); };
    std::sort(active_live_intervals_.begin(), active_live_intervals_.end(), comparator);
}