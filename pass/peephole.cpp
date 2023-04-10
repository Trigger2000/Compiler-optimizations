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
    for (auto user : old_inst->GetUsers()) {
        new_inst->AddUser(user);
        user->SubstituteInput(old_inst, new_inst);
    }
}

void Peephole::VisitSUB(Inst* inst)
{
    assert(inst->GetOpcode() == Opcode::SUB);
    auto inst_casted = inst->CastToInstWithTwoInputs();

    // case 1
    // 1 sub v0 0
    // ----------
    // users(v1) = users(v0)
    if (inst_casted->GetInput2()->GetOpcode() == Opcode::CONSTANT &&
        inst_casted->GetInput2()->CastToInstConstant()->GetConstant() == 0) {
        auto input1 = inst_casted->GetInput1();
        ProcessUsersInputs(inst, input1);
        inst->SetBB(nullptr);
    }

    // case 2
    // 1 sub v0 v0
    // -----------
    // 2 constant 0
    // users(v1) = users(v2)
    if (inst_casted->GetInput1() == inst_casted->GetInput2()) {
        auto new_inst = Inst::InstBuilder<Opcode::CONSTANT>(Inst::NextId());
        new_inst->CastToInstConstant()->SetConstant(0);
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
    if (inst_casted->GetInput1() == inst->GetPrev() &&
        inst->GetPrev()->GetOpcode() == Opcode::ADD &&
        inst->GetPrev()->CastToInstWithTwoInputs()->GetInput2() == inst_casted->GetInput2()) {
        auto input1_prev = inst->GetPrev()->CastToInstWithTwoInputs()->GetInput1();
        ProcessUsersInputs(inst, input1_prev);
        inst->SetBB(nullptr);
    }

    // case 4
    // 2. sub v1 v0
    // 3. sub v1 v2
    // ------------
    // 2. sub v1 v0
    // users(v3) = users(v0)
    if (inst_casted->GetInput2() == inst->GetPrev() &&
        inst->GetPrev()->GetOpcode() == Opcode::SUB &&
        inst->GetPrev()->CastToInstWithTwoInputs()->GetInput1() == inst_casted->GetInput1()) {
        auto input2_prev = inst->GetPrev()->CastToInstWithTwoInputs()->GetInput2();
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
    if (inst_casted->GetInput1() == inst->GetPrev() &&
        inst->GetPrev()->GetOpcode() == Opcode::SUB &&
        inst_casted->GetInput2()->GetOpcode() == Opcode::CONSTANT &&
        inst->GetPrev()->CastToInstWithTwoInputs()->GetInput2()->GetOpcode() == Opcode::CONSTANT) {
        int32_t new_const = inst_casted->GetInput2()->CastToInstConstant()->GetConstant() +
                            inst->GetPrev()->CastToInstWithTwoInputs()->GetInput2()->CastToInstConstant()->GetConstant();
        auto new_inst = Inst::InstBuilder<Opcode::CONSTANT>(Inst::NextId());
        new_inst->CastToInstConstant()->SetConstant(new_const);
        inst->GetBB()->PushFrontInst(new_inst);
        new_inst->AddUser(inst);

        inst_casted->GetInput2()->RemoveUser(inst);
        if (inst_casted->GetInput2()->GetUsers().size() == 0)
            inst_casted->GetInput2()->SetBB(nullptr);

        inst_casted->GetInput1()->RemoveUser(inst);
        if (inst_casted->GetInput1()->GetUsers().size() == 0)
            inst_casted->GetInput1()->SetBB(nullptr);

        inst->SubstituteInput(inst_casted->GetInput2(), new_inst);
        inst->SubstituteInput(inst_casted->GetInput1(), inst->GetPrev()->CastToInstWithTwoInputs()->GetInput1());
    }
}

void Peephole::VisitSHR(Inst* inst)
{
    assert(inst->GetOpcode() == Opcode::SHR);
    auto inst_casted = inst->CastToInstWithTwoInputs();

    // case 1
    // 1 shr v0 0
    // ----------
    // users(v1) = users(v0)
    if (inst_casted->GetInput2()->GetOpcode() == Opcode::CONSTANT &&
        inst_casted->GetInput2()->CastToInstConstant()->GetConstant() == 0) {
        auto input1 = inst_casted->GetInput1();
        ProcessUsersInputs(inst, input1);
        inst->SetBB(nullptr);
    }

    // case 2
    // 2 shl v0 v1
    // 3 shr v2 v1
    // -----------
    // 2 shl v0 v1
    // users(v3) = users(v0) 
    if (inst_casted->GetInput1() == inst->GetPrev() &&
        inst->GetPrev()->GetOpcode() == Opcode::SHL &&
        inst->GetPrev()->CastToInstWithTwoInputs()->GetInput2() == inst_casted->GetInput2()) {
        auto input1_prev = inst->GetPrev()->CastToInstWithTwoInputs()->GetInput1();
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
    if (inst_casted->GetInput1() == inst->GetPrev() &&
        inst->GetPrev()->GetOpcode() == Opcode::SHR &&
        inst_casted->GetInput2()->GetOpcode() == Opcode::CONSTANT &&
        inst->GetPrev()->CastToInstWithTwoInputs()->GetInput2()->GetOpcode() == Opcode::CONSTANT) {
        int32_t new_const = inst_casted->GetInput2()->CastToInstConstant()->GetConstant() +
                            inst->GetPrev()->CastToInstWithTwoInputs()->GetInput2()->CastToInstConstant()->GetConstant();
        auto new_inst = Inst::InstBuilder<Opcode::CONSTANT>(Inst::NextId());
        new_inst->CastToInstConstant()->SetConstant(new_const);
        inst->GetBB()->PushFrontInst(new_inst);
        new_inst->AddUser(inst);

        inst_casted->GetInput2()->RemoveUser(inst);
        if (inst_casted->GetInput2()->GetUsers().size() == 0)
            inst_casted->GetInput2()->SetBB(nullptr);

        inst_casted->GetInput1()->RemoveUser(inst);
        if (inst_casted->GetInput1()->GetUsers().size() == 0)
            inst_casted->GetInput1()->SetBB(nullptr);

        inst->SubstituteInput(inst_casted->GetInput2(), new_inst);
        inst->SubstituteInput(inst_casted->GetInput1(), inst->GetPrev()->CastToInstWithTwoInputs()->GetInput1());
    }
}

void Peephole::VisitXOR(Inst* inst)
{
    assert(inst->GetOpcode() == Opcode::XOR);
    auto inst_casted = inst->CastToInstWithTwoInputs();

    // case 1
    // 1 xor v0 0
    // ----------
    // users(v1) = users(v0)
    if (inst_casted->GetInput2()->GetOpcode() == Opcode::CONSTANT &&
        inst_casted->GetInput2()->CastToInstConstant()->GetConstant() == 0) {
        auto input1 = inst_casted->GetInput1();
        ProcessUsersInputs(inst, input1);
        inst->SetBB(nullptr);
    }

    // case 2
    // 1 xor v0 v0
    // -----------
    // 2 constant 0
    // users(v1) = users(v2)
    if (inst_casted->GetInput1() == inst_casted->GetInput2()) {
        auto new_inst = Inst::InstBuilder<Opcode::CONSTANT>(Inst::NextId());
        new_inst->CastToInstConstant()->SetConstant(0);
        inst->GetBB()->PushFrontInst(new_inst);
        ProcessUsersInputs(inst, new_inst);
        inst->SetBB(nullptr);
    }
    // case 3
    // xor v0 -1
    // ---------
    // not v0
    if (inst_casted->GetInput2()->GetOpcode() == Opcode::CONSTANT &&
        inst_casted->GetInput2()->CastToInstConstant()->GetConstant() == -1) {

        auto new_inst = Inst::InstBuilder<Opcode::NOT>(Inst::NextId());
        new_inst->CastToInstWithOneInput()->SetInput1(inst_casted->GetInput1());
        inst->GetBB()->InsertInst(inst, new_inst);
        inst_casted->GetInput1()->RemoveUser(inst);
        inst_casted->GetInput1()->AddUser(new_inst);

        ProcessUsersInputs(inst, new_inst);

        inst->SetBB(nullptr);
    }
}
