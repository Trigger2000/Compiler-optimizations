#include "peephole.h"
#include "rpo.h"
#include "dce.h"

void Peephole::RunPassImpl(Graph *g)
{
    g->RunPass<RPO>();

    auto rpo_bbs = g->GetRPOBasicBlocks();
    for (BasicBlock* bb: rpo_bbs) {
        for (Inst *inst = bb->GetFirstInst(); inst != nullptr; inst = inst->GetNext()) {
            table_[static_cast<size_t>(inst->GetOpcode())](inst);
        }
    }

    g->SetPassValidity<DCE>(false);
    g->RunPass<DCE>();
}