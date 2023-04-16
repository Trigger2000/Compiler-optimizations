#ifndef LINEAR_ORDER_H
#define LINEAR_ORDER_H

#include "ir/graph.h"

class LinearOrder {
public:
    void RunPassImpl(Graph *g);

private:
    void ProccessLoop(Loop* loop, marker visit_marker);

    void FixFalseBranchOrder(BasicBlock* bb);
    void CheckFalseBranchOrder(BasicBlock* bb);
    Opcode GetInverseCondition(Opcode op);

    std::vector<BasicBlock*> linear_order_;
};

#endif // LIVENESS_ANALYSIS_H
