#ifndef DCE_H
#define DCE_H

#include "ir/graph.h"

class DCE {
public:
    void RunPassImpl(Graph *g);
};

#endif // DCE_H
