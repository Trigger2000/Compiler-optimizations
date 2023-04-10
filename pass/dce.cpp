#include "dce.h"

void DCE::RunPassImpl(Graph *g)
{
    // mark
    marker sweep_marker = g->NewMarker();
    for (BasicBlock* bb: g->GetBasicBlocks()) {
        for (Inst *inst = bb->GetFirstInst(); inst != nullptr; inst = inst->GetNext()) {
            if (inst->GetBB() == nullptr)
                MarkRecursively(inst, sweep_marker);
        }
    }

    // sweep
    for (BasicBlock* bb: g->GetBasicBlocks()) {
        for (Inst *inst = bb->GetFirstInst(); inst != nullptr;) {
            Inst *next = inst->GetNext();
            if (inst->IsMarked(sweep_marker)) {
                DeleteInst(inst, bb);
            }
            inst = next;
        }
    }
    g->EraseMarker(sweep_marker);
}

void DCE::MarkRecursively(Inst* inst, marker sweep_marker)
{
    if (inst->IsMarked(sweep_marker)) {
        return;
    }
    inst->SetMarker(sweep_marker);

    if (inst->GetType() != Type::InstWithTwoInputs)
        return;

    auto input1 = inst->CastToInstWithTwoInputs()->GetInput1();
    auto input2 = inst->CastToInstWithTwoInputs()->GetInput2();

    input1->RemoveUser(inst);
    if (input1->GetUsers().size() == 0) {
        MarkRecursively(input1, sweep_marker);
    }

    if (input1 == input2)
        return;
    
    input2->RemoveUser(inst);
    if (input2->GetUsers().size() == 0) {
        MarkRecursively(input2, sweep_marker);
    }
}

void DCE::DeleteInst(Inst* inst, BasicBlock* bb)
{
    if (bb->GetFirstInst() == inst)
        bb->SetFirstInst(inst->GetNext());
    if (bb->GetLastInst() == inst)
        bb->SetLastInst(inst->GetPrev());
    bb->SetSize(bb->GetSize() - 1);
    delete inst;
}
