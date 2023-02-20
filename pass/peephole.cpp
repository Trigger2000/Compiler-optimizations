#include "peephole.h"
#include "dce.h"
#include "rpo.h"

void Peephole::RunPassImpl(Graph* g)
{
    g->RunPass<RPO>();

    auto rpo_bbs = g->GetRPOBasicBlocks();
    for (BasicBlock* bb : rpo_bbs) {
        for (Inst* inst = bb->GetFirstInst(); inst != nullptr; inst = inst->GetNext()) {
            table_[static_cast<size_t>(inst->GetOpcode())](inst);
        }
    }

    g->SetPassValidity<DCE>(false);
    g->RunPass<DCE>();
}

void Peephole::ProcessUsersInputs(Inst *old_inst, Inst *new_inst)
{
    for (auto user : old_inst->GetUsers().GetUsers()) {
        new_inst->GetUsers().AddUser(user);
        user->SubstituteInput(old_inst, new_inst);
    }
}

void Peephole::VisitSUB(Inst* inst)
{
    assert(inst->GetOpcode() == Opcode::SUB);

    // case 1
    // 1 sub v0 0
    // ----------
    // users(v1) = users(v0)
    if (inst->GetInput2()->GetInputInst()->GetOpcode() == Opcode::CONSTANT &&
        inst->GetInput2()->GetInputInst()->GetConstant() == 0) {
        auto input1 = inst->GetInput1()->GetInputInst();
        ProcessUsersInputs(inst, input1);
        inst->SetBB(nullptr);
    }

    // case 2
    // 1 sub v0 v0
    // -----------
    // 2 constant 0
    // users(v1) = users(v2)
    if (inst->GetInput1()->GetInputInst() == inst->GetInput2()->GetInputInst()) {
        auto new_inst = Inst::InstBuilder<Opcode::CONSTANT>(Inst::NextId(), 0);
        inst->GetBB()->PushFrontInst(new_inst);
        ProcessUsersInputs(inst, new_inst);
        inst->SetBB(nullptr);
    }

    // case 3
    // 2 add v0 v1
    // 3 sub v2 v1
    // -----------
    // 2 add v0 v1
    // users(v3) = users(v0) 
    if (inst->GetInput1()->GetInputInst() == inst->GetPrev() &&
        inst->GetPrev()->GetOpcode() == Opcode::ADD &&
        inst->GetPrev()->GetInput2()->GetInputInst() == inst->GetInput2()->GetInputInst()) {
        auto input1_prev = inst->GetPrev()->GetInput1()->GetInputInst();
        ProcessUsersInputs(inst, input1_prev);
        inst->SetBB(nullptr);
    }

    // case 4
    // 2. sub v1 v0
    // 3. sub v1 v2
    // ------------
    // 2. sub v1 v0
    // users(v3) = users(v0)
    if (inst->GetInput2()->GetInputInst() == inst->GetPrev() &&
        inst->GetPrev()->GetOpcode() == Opcode::SUB &&
        inst->GetPrev()->GetInput1()->GetInputInst() == inst->GetPrev()->GetInput1()->GetInputInst()) {
        auto input2_prev = inst->GetPrev()->GetInput2()->GetInputInst();
        ProcessUsersInputs(inst, input2_prev);
        inst->SetBB(nullptr);
    }

    // case 5
    // 2. sub v1 const1
    // 3. sub v2 const2
    // ----------------
    // const3 = const1 + const2
    // 2. sub v1 const1
    // 3. sub v1 const3
    if (inst->GetInput1()->GetInputInst() == inst->GetPrev() &&
        inst->GetPrev()->GetOpcode() == Opcode::SUB &&
        inst->GetInput2()->GetInputInst()->GetOpcode() == Opcode::CONSTANT &&
        inst->GetPrev()->GetInput2()->GetInputInst()->GetOpcode() == Opcode::CONSTANT) {
        int32_t new_const = inst->GetInput2()->GetInputInst()->GetConstant() +
                            inst->GetPrev()->GetInput2()->GetInputInst()->GetConstant();
        auto new_inst = Inst::InstBuilder<Opcode::CONSTANT>(Inst::NextId(), new_const);
        inst->GetBB()->PushFrontInst(new_inst);
        new_inst->GetUsers().AddUser(inst);

        inst->GetInput2()->GetInputInst()->GetUsers().RemoveUser(inst);
        if (inst->GetInput2()->GetInputInst()->GetUsers().GetUsers().size() == 0)
            inst->GetInput2()->GetInputInst()->SetBB(nullptr);

        inst->GetInput1()->GetInputInst()->GetUsers().RemoveUser(inst);
        if (inst->GetInput1()->GetInputInst()->GetUsers().GetUsers().size() == 0)
            inst->GetInput1()->GetInputInst()->SetBB(nullptr);

        inst->SubstituteInput(inst->GetInput2()->GetInputInst(), new_inst);
        inst->SubstituteInput(inst->GetInput1()->GetInputInst(), inst->GetPrev()->GetInput1()->GetInputInst());
    }
}

void Peephole::VisitSHR(Inst* inst)
{
    assert(inst->GetOpcode() == Opcode::SHR);

    // case 1
    // 1 shr v0 0
    // ----------
    // users(v1) = users(v0)
    if (inst->GetInput2()->GetInputInst()->GetOpcode() == Opcode::CONSTANT &&
        inst->GetInput2()->GetInputInst()->GetConstant() == 0) {
        auto input1 = inst->GetInput1()->GetInputInst();
        ProcessUsersInputs(inst, input1);
        inst->SetBB(nullptr);
    }

    // case 2
    // 2 shl v0 v1
    // 3 shr v2 v1
    // -----------
    // 2 shl v0 v1
    // users(v3) = users(v0) 
    if (inst->GetInput1()->GetInputInst() == inst->GetPrev() &&
        inst->GetPrev()->GetOpcode() == Opcode::SHL &&
        inst->GetPrev()->GetInput2()->GetInputInst() == inst->GetInput2()->GetInputInst()) {
        auto input1_prev = inst->GetPrev()->GetInput1()->GetInputInst();
        ProcessUsersInputs(inst, input1_prev);
        inst->SetBB(nullptr);
    }

    // case 3
    // 2. shr v1 const1
    // 3. shr v2 const2
    // ----------------
    // const3 = const1 + const2
    // 2. shr v1 const1
    // 3. shr v1 const3
    if (inst->GetInput1()->GetInputInst() == inst->GetPrev() &&
        inst->GetPrev()->GetOpcode() == Opcode::SHR &&
        inst->GetInput2()->GetInputInst()->GetOpcode() == Opcode::CONSTANT &&
        inst->GetPrev()->GetInput2()->GetInputInst()->GetOpcode() == Opcode::CONSTANT) {
        int32_t new_const = inst->GetInput2()->GetInputInst()->GetConstant() +
                            inst->GetPrev()->GetInput2()->GetInputInst()->GetConstant();
        auto new_inst = Inst::InstBuilder<Opcode::CONSTANT>(Inst::NextId(), new_const);
        inst->GetBB()->PushFrontInst(new_inst);
        new_inst->GetUsers().AddUser(inst);

        inst->GetInput2()->GetInputInst()->GetUsers().RemoveUser(inst);
        if (inst->GetInput2()->GetInputInst()->GetUsers().GetUsers().size() == 0)
            inst->GetInput2()->GetInputInst()->SetBB(nullptr);

        inst->GetInput1()->GetInputInst()->GetUsers().RemoveUser(inst);
        if (inst->GetInput1()->GetInputInst()->GetUsers().GetUsers().size() == 0)
            inst->GetInput1()->GetInputInst()->SetBB(nullptr);

        inst->SubstituteInput(inst->GetInput2()->GetInputInst(), new_inst);
        inst->SubstituteInput(inst->GetInput1()->GetInputInst(), inst->GetPrev()->GetInput1()->GetInputInst());
    }
}

void Peephole::VisitXOR(Inst* inst)
{
    assert(inst->GetOpcode() == Opcode::XOR);

    // case 1
    // 1 xor v0 0
    // ----------
    // users(v1) = users(v0)
    if (inst->GetInput2()->GetInputInst()->GetOpcode() == Opcode::CONSTANT &&
        inst->GetInput2()->GetInputInst()->GetConstant() == 0) {
        auto input1 = inst->GetInput1()->GetInputInst();
        ProcessUsersInputs(inst, input1);
        inst->SetBB(nullptr);
    }

    // case 2
    // 1 xor v0 v0
    // -----------
    // 2 constant 0
    // users(v1) = users(v2)
    if (inst->GetInput1()->GetInputInst() == inst->GetInput2()->GetInputInst()) {
        auto new_inst = Inst::InstBuilder<Opcode::CONSTANT>(Inst::NextId(), 0);
        inst->GetBB()->PushFrontInst(new_inst);
        ProcessUsersInputs(inst, new_inst);
        inst->SetBB(nullptr);
    }

    // case 3
    // xor v0 -1
    // ---------
    // not v0
    if (inst->GetInput2()->GetInputInst()->GetOpcode() == Opcode::CONSTANT &&
        inst->GetInput2()->GetInputInst()->GetConstant() == -1) {

        auto new_inst = Inst::InstBuilder<Opcode::NOT>(Inst::NextId(), inst->GetInput1()->GetInputId());
        inst->GetBB()->InsertInst(inst, new_inst);
        new_inst->GetInput1()->SetInputInst(inst->GetInput1()->GetInputInst());
        inst->GetInput1()->GetInputInst()->GetUsers().RemoveUser(inst);
        inst->GetInput1()->GetInputInst()->GetUsers().AddUser(new_inst);

        ProcessUsersInputs(inst, new_inst);

        inst->SetBB(nullptr);
    }
}
