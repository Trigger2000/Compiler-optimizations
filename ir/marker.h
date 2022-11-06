#ifndef MARKER_H
#define MARKER_H

#include <bitset>
#include <cassert>

class Markers {
public:
    void SetMarker(size_t pos, bool value)
    {
        assert(pos < markers_.size());
        markers_[pos] = value;
    }

    bool GetMarker(size_t pos)
    {
        assert(pos < markers_.size());
        return markers_[pos];
    }

    bool ClearMarkers()
    {
        markers_.reset();
    }

private:
    std::bitset<32> markers_;
};

#endif  // MARKER_H
