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

bool ConstFolding::CheckConstInput(InstWithTwoInputs *inst)
{
    return inst->GetInput1()->GetOpcode() == Opcode::CONSTANT &&
           inst->GetInput2()->GetOpcode() == Opcode::CONSTANT;
}

void ConstFolding::CreateNewConstant(Inst* old_inst, int32_t constant)
{
    Inst* new_inst = Inst::InstBuilder<Opcode::CONSTANT>(Inst::NextId());
    static_cast<InstConstant*>(new_inst)->SetConstant(constant);
    old_inst->GetBB()->PushFrontInst(new_inst);
    for (auto user: old_inst->GetUsers()) {
        new_inst->AddUser(user);
        user->SubstituteInput(old_inst, new_inst);
    }
    old_inst->SetBB(nullptr);
}

void ConstFolding::VisitSUB(Inst *inst)
{
    InstWithTwoInputs* inst_casted = inst->CastToInstWithTwoInputs();
    if (!CheckConstInput(inst_casted)) {
        return;
    }
    assert(inst->GetOpcode() == Opcode::SUB);
    int32_t fold_result = inst_casted->GetInput1()->CastToInstConstant()->GetConstant() -
                          inst_casted->GetInput2()->CastToInstConstant()->GetConstant();
    CreateNewConstant(inst, fold_result);
}

void ConstFolding::VisitSHR(Inst *inst)
{
    InstWithTwoInputs* inst_casted = inst->CastToInstWithTwoInputs();
    if (!CheckConstInput(inst_casted)) {
        return;
    }
    assert(inst->GetOpcode() == Opcode::SHR);
    int32_t fold_result = inst_casted->GetInput1()->CastToInstConstant()->GetConstant() >>
                          inst_casted->GetInput2()->CastToInstConstant()->GetConstant();
    CreateNewConstant(inst, fold_result);
}

void ConstFolding::VisitXOR(Inst *inst)
{
    InstWithTwoInputs* inst_casted = static_cast<InstWithTwoInputs*>(inst);
    if (!CheckConstInput(inst_casted)) {
        return;
    }
    assert(inst->GetOpcode() == Opcode::XOR);
    int32_t fold_result = inst_casted->GetInput1()->CastToInstConstant()->GetConstant() ^
                          inst_casted->GetInput2()->CastToInstConstant()->GetConstant();
    CreateNewConstant(inst, fold_result);
}
