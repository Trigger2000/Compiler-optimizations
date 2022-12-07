#include "const_folding.h"
#include "rpo.h"
#include "dce.h"

void ConstFolding::RunPassImpl(Graph *g)
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

bool ConstFolding::CheckConstInput(Inst *inst)
{
    return inst->GetInput1()->GetInputInst()->GetOpcode() == Opcode::CONSTANT &&
           inst->GetInput2()->GetInputInst()->GetOpcode() == Opcode::CONSTANT;
}

void ConstFolding::CreateNewConstant(Inst* old_inst, int32_t constant)
{
    Inst* new_inst = Inst::InstBuilder(Opcode::CONSTANT, constant);
    old_inst->GetBB()->PushFrontInst(new_inst);
    for (auto user: old_inst->GetUsers().GetUsers()) {
        new_inst->GetUsers().AddUser(user);
        user->SubstituteInput(old_inst, new_inst);
    }

    auto input1 = old_inst->GetInput1()->GetInputInst();
    auto input2 = old_inst->GetInput2()->GetInputInst();
    input1->GetUsers().RemoveUser(old_inst);
    if (input1->GetUsers().GetUsers().size() == 0) {
        input1->SetBB(nullptr);
    }
    input2->GetUsers().RemoveUser(old_inst);
    if (input2->GetUsers().GetUsers().size() == 0) {
        input2->SetBB(nullptr);
    }
    old_inst->SetBB(nullptr);
}

void ConstFolding::VisitSUB(Inst *inst)
{
    if (!CheckConstInput(inst)) {
        return;
    }
    assert(inst->GetOpcode() == Opcode::SUB);
    int32_t fold_result = inst->GetInput1()->GetInputInst()->GetConstant() -
                          inst->GetInput2()->GetInputInst()->GetConstant();
    CreateNewConstant(inst, fold_result);
}

void ConstFolding::VisitSHR(Inst *inst)
{
    if (!CheckConstInput(inst)) {
        return;
    }
    assert(inst->GetOpcode() == Opcode::SHR);
    int32_t fold_result = inst->GetInput1()->GetInputInst()->GetConstant() >>
                          inst->GetInput2()->GetInputInst()->GetConstant();
    CreateNewConstant(inst, fold_result);
}

void ConstFolding::VisitXOR(Inst *inst)
{
    if (!CheckConstInput(inst)) {
        return;
    }
    assert(inst->GetOpcode() == Opcode::XOR);
    int32_t fold_result = inst->GetInput1()->GetInputInst()->GetConstant() ^
                          inst->GetInput2()->GetInputInst()->GetConstant();
    CreateNewConstant(inst, fold_result);
}
