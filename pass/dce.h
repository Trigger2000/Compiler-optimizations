#ifndef DCE_H
#define DCE_H

#include "ir/graph.h"

class DCE {
public:
    void RunPassImpl(Graph *g);

private:
    void MarkRecursively(Inst* inst, marker mrk);
    void DeleteInst(Inst* inst, BasicBlock* bb);
};

#endif // DCE_H
