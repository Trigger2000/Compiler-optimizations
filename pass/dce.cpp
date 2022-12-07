#include "dce.h"

void DCE::RunPassImpl(Graph *g)
{
    for (BasicBlock* bb: g->GetBasicBlocks()) {
        for (Inst *inst = bb->GetFirstInst(); inst != nullptr;) {
            Inst *next = inst->GetNext();
            if (inst->GetBB() == nullptr) {
                delete inst;
                bb->SetSize(bb->GetSize() - 1);
            }
            inst = next;
        }
        // TODO add basic block deletion
    }
}
