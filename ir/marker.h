#ifndef MARKER_H
#define MARKER_H

#include <array>
#include <cassert>
#include <bitset>
#include <limits>

#include "utils.h"

using marker = uint32_t;
constexpr uint8_t MARKER_NUM = 4;
constexpr uint8_t SLOT_BITS = 4;
constexpr uint8_t SLOT_MASK = 0b1111;
constexpr marker MAX_EPOCH = std::numeric_limits<uint32_t>::max() & 0b10000;

class MarkerManager {
public:
    marker NewMarker() {
        current_epoch_++;
        for (int i = 0; i < slots_.size(); ++i) {
            if (!slots_[i]) {
                assert(current_epoch_ < MAX_EPOCH);
                slots_[i] = true;
                marker mrk = (current_epoch_ << SLOT_BITS) | i;
                return mrk;
            }
        }
        UNREACHABLE()
    }

    void EraseMarker(marker mrk) {
        uint8_t slot = mrk & SLOT_MASK;
        slots_[slot] = 0;
    }

private:
    uint32_t current_epoch_ = 0;
    std::bitset<MARKER_NUM> slots_;
};

class Markers {
public:
    void SetMarker(marker mrk)
    {
        uint8_t slot = mrk & SLOT_MASK;
        uint32_t epoch = mrk >> SLOT_BITS;
        markers_[slot] = epoch;
    }

    void ResetMarker(marker mrk)
    {
        uint8_t slot = mrk & SLOT_MASK;
        markers_[slot] = 0;
    }

    bool IsMarked(marker mrk)
    {
        uint32_t slot = mrk & SLOT_MASK;
        uint32_t epoch = mrk >> SLOT_BITS;
        return markers_[slot] == epoch;
    }

private:
    std::array<marker, MARKER_NUM> markers_;
};

#endif  // MARKER_H
