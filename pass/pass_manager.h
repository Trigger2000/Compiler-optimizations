#ifndef PASS_MANAGER_H
#define PASS_MANAGER_H

#include "ir/graph.h"

class PassManager {
public:
    template <typename T>
    void RunPass(Graph *g)
    {
        T pass;
        pass.RunPassImpl(g);
    }
};

#endif // PASS_MANAGER_H
