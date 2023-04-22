#ifndef LIVE_SET_H
#define LIVE_SET_H

#include <algorithm>
#include <cstdint>
#include <set>

#include "utils.h"
#include "inst.h"

class LiveSet
{
public:
    LiveSet() = default;
    ACCESSOR_MUTATOR(live_set_, LiveSet, std::set<Inst*>&)

    void AddInst(Inst* inst)
    {
        live_set_.insert(inst);
    }

    void Union(LiveSet& live_set)
    {
        for (auto item: live_set.GetLiveSet()) {
            live_set_.insert(item);
        }
    }

    void RemoveInst(Inst* inst)
    {
        live_set_.erase(inst);
    }

private:
    std::set<Inst*> live_set_;
};

class LiveInterval
{
public:
    LiveInterval() = default;
    LiveInterval(uint32_t start, uint32_t end) : start_(start), end_(end) {}
    ACCESSOR_MUTATOR(start_, Start, uint32_t)
    ACCESSOR_MUTATOR(end_, End, uint32_t)
    ACCESSOR_MUTATOR(location_, Location, uint32_t)
    ACCESSOR_MUTATOR(is_stack_location_, IsStackLocation, bool)
    

    void AddInterval(uint32_t start, uint32_t end)
    {
        start_ = std::min(start, start_);
        end_ = std::max(end, end_);
    }

private:
    uint32_t start_ = 0;
    uint32_t end_ = 0;

    uint32_t location_ = 0;
    bool is_stack_location_ = false;
};


#endif // LIVE_SET_H
