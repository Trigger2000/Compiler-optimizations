#ifndef REG_ALLOC_H
#define REG_ALLOC_H

#include <set>

#include "ir/graph.h"

class RegAlloc {
public:
    void RunPassImpl(Graph* g);

    static void SetRegCount(size_t reg_num)
    {
        assert(reg_num <= MAX_REG_NUM);
        reg_num_ = reg_num;
    }

private:
    void PrepareIntervals(Graph* g);
    void SortActiveIntervals();

    void LinearScan();
    void ExpireOldIntervals(LiveInterval* cur_interval);
    void SpillAtInterval(LiveInterval* cur_interval);

    void FreeReg(size_t index)
    {
        reg_map_[index] = 0;
    }

    size_t ReserveReg()
    {
        int free_reg = 0;
        for (; reg_map_[free_reg] == 1; ++free_reg);
        reg_map_[free_reg] = 1;
        return free_reg;
    }

    static constexpr size_t MAX_REG_NUM = 31;
    static inline size_t reg_num_ = MAX_REG_NUM;

    std::vector<LiveInterval*> live_intervals_;
    std::vector<LiveInterval*> active_live_intervals_;
    std::bitset<MAX_REG_NUM> reg_map_;

    uint32_t cur_free_stack_slot_ = 0;
};

#endif // REG_ALLOC_H