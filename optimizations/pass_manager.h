#ifndef PASS_MANAGER_H
#define PASS_MANAGER_H

#include "ir/graph.h"

class PassManager {
public:
    PassManager(Graph *g) : g_(g) {}

    void RunPass();
private:
    Graph *g_;
};

#endif // PASS_MANAGER_H