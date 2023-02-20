#ifndef INLINING_H
#define INLINING_H

#include "ir/graph.h"

class Inlining {
public:
    void RunPassImpl(Graph *g);

private:
    void InlineStaticMethod(Inst* call_inst);

    void SubstituteUsersInputsForArgs(Graph* callee, Inst* call_inst);
    std::vector<BasicBlock*> ProcessReturns(Graph* callee, Inst* call_inst);
    void MoveConstants(Graph* callee, Inst* call_inst);
    void SplitMoveAndConnectBlocks(Graph* callee, Inst* call_inst, const std::vector<BasicBlock*>& callee_ret_bbs);
};

#endif // INLINING_H
