#ifndef LIVE_INTERVAL_H
#define LIVE_INTERVAL_H

#include <algorithm>
#include <cstdint>

#include "utils.h"

class LiveInterval
{
public:
    LiveInterval() = default;
    LiveInterval(uint32_t start, uint32_t end) : start_(start), end_(end) {}
    ACCESSOR_MUTATOR(start_, Start, uint32_t)
    ACCESSOR_MUTATOR(end_, End, uint32_t)

    LiveInterval Union(const LiveInterval& interval)
    {
        if (start_ == 0 && end_ == 0) {
            return interval;
        }
        return LiveInterval(std::min(start_, interval.start_), std::max(end_, interval.end_));
    }
private:
    uint32_t start_ = 0;
    uint32_t end_ = 0;
};

#endif // LIVE_INTERVAL_H
