#ifndef DOM_TREE_SLOW_H
#define DOM_TREE_SLOW_H

#include "pass.h"
#include "rpo.h"

class DomTreeSlow {
public:
    void RunPassImpl(Graph *g);

private:
    void BindBasicBlock(BasicBlock* bb)
    {
        bb->SetMarker(BB_DELETED_MARKER, false);
    }

    void UnbindBasicBlock(BasicBlock* bb)
    {
        bb->SetMarker(BB_DELETED_MARKER, true);
    }

    std::vector<BasicBlock*> basic_blocks_;

    const int BB_DELETED_MARKER = 1;
};

#endif // DOM_TREE_SLOW_H
