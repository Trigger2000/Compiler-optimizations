#ifndef RPO_H
#define RPO_H

#include "ir/graph.h"
#include "../ir/marker.h"

class RPO final {
public:
    void RunPassImpl(Graph *g);

private:
    void PostOrderVisitor(std::vector<BasicBlock*>& result, BasicBlock* current, marker visited_marker);

    std::vector<BasicBlock*> basic_blocks_;
};

#endif // RPO_H
