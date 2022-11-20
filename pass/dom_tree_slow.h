#ifndef DOM_TREE_SLOW_H
#define DOM_TREE_SLOW_H

#include "ir/graph.h"
#include "rpo.h"

class DomTreeSlow {
public:
    void RunPassImpl(Graph *g);

private:
    std::vector<BasicBlock*> basic_blocks_;
};

#endif // DOM_TREE_SLOW_H
