#ifndef CHECK_ELIMINATION_H
#define CHECK_ELIMINATION_H

#include "ir/graph.h"

class CheckElimination {
public:
    void RunPassImpl(Graph *g);

private:
    void TryEliminateCheckOneInput(Inst* inst, Graph *g);
    void TryEliminateCheckTwoInput(Inst* inst, Graph *g);

    bool CheckInputsEqual(InstWithTwoInputs* inst1, InstWithTwoInputs* inst2);
};

#endif // CHECK_ELIMINATION_H
