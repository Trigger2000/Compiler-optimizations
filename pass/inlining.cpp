#include "inlining.h"
#include "rpo.h"

void Inlining::RunPassImpl(Graph* g)
{
    auto bbs = g->GetBasicBlocks();
    for (BasicBlock* bb: bbs) {
        for (Inst *inst = bb->GetFirstInst(); inst != nullptr; inst = inst->GetNext()) {
            // suppose it's better to save list of call instructions while building graph
            // but for the first time let's be primitive
            if (inst->GetOpcode() == Opcode::CALL_STATIC) {
                InlineStaticMethod(inst);
            }
        }
    }
}

void Inlining::InlineStaticMethod(Inst* call_inst)
{
    Graph* callee = call_inst->CastToInstCall()->GetCallee();

    SubstituteUsersInputsForArgs(callee, call_inst);

    std::vector<BasicBlock*> callee_ret_bbs = ProcessReturns(callee, call_inst);

    MoveConstants(callee, call_inst);

    SplitMoveAndConnectBlocks(callee, call_inst, callee_ret_bbs);
    
    call_inst->GetBB()->PopBackInst();
    callee->Clear();
}

void Inlining::SubstituteUsersInputsForArgs(Graph* callee, Inst* call_inst)
{
    // substitute users and inputs for arguments
    for (auto arg: call_inst->CastToInstCall()->GetArguments()) {
        Inst* callee_param = callee->GetBasicBlocks()[0]->GetFirstInst();
        arg->RemoveUser(call_inst);
        for (auto callee_param_user: callee_param->GetUsers()) {
            arg->AddUser(callee_param_user);
            callee_param_user->SubstituteInput(callee_param, arg);
        }
        callee->GetBasicBlocks()[0]->PopFrontInst();
    }
}

std::vector<BasicBlock*> Inlining::ProcessReturns(Graph* callee, Inst* call_inst)
{
    BasicBlock* caller_inst_bb = call_inst->GetBB();
    // find all ret instructions
    std::vector<Inst*> returns;
    int ret_counter = 0;
    for (auto bb_callee: callee->GetBasicBlocks()) {
        if (bb_callee->GetLastInst()->GetOpcode() == Opcode::RET || bb_callee->GetLastInst()->GetOpcode() == Opcode::RET_VOID ||
            bb_callee->GetLastInst()->GetOpcode() == Opcode::THROW) {
            if (bb_callee->GetLastInst()->GetOpcode() == Opcode::RET)
                ret_counter++;
            returns.push_back(bb_callee->GetLastInst());
        }
    }
    // for not ret_void instructions subsittute users
    if (ret_counter > 0) {
        Inst* call_result_inst = nullptr;
        if (ret_counter > 1) {
            call_result_inst = Inst::InstBuilder<Opcode::PHI>(Inst::NextId());
            for (auto ret_inst: returns) {
                if (ret_inst->GetOpcode() != Opcode::THROW) {
                    auto ret_inst_casted = ret_inst->CastToInstWithOneInput();
                    call_result_inst->CastToInstPhi()->AddInput(ret_inst_casted->GetInput1(), ret_inst_casted->GetInput1()->GetBB());
                    ret_inst_casted->GetInput1()->AddUser(call_result_inst);
                }
            }
            if (call_inst->IsEndInst()) {
                caller_inst_bb->PushBackInst(call_result_inst);
            } else {
                caller_inst_bb->InsertInst(call_inst->GetNext(), call_result_inst);
            }
        } else {
            call_result_inst = returns[0]->CastToInstWithOneInput()->GetInput1();
        }
        for (auto user: call_inst->GetUsers()) {
            call_result_inst->AddUser(user);
            user->SubstituteInput(call_inst, call_result_inst);
        }
    }
    // remove ret instructions from callee graph
    std::vector<BasicBlock*> callee_ret_bbs;
    for (auto bb_callee: callee->GetBasicBlocks()) {
        if (bb_callee->GetLastInst()->GetOpcode() == Opcode::RET ||
            bb_callee->GetLastInst()->GetOpcode() == Opcode::RET_VOID) {
            if (bb_callee->GetLastInst()->GetOpcode() == Opcode::RET) {
                bb_callee->GetLastInst()->CastToInstWithOneInput()->GetInput1()->RemoveUser(bb_callee->GetLastInst());
            }
            bb_callee->PopBackInst();
            callee_ret_bbs.push_back(bb_callee);
        }
        if (bb_callee->GetLastInst()->GetOpcode() == Opcode::THROW) {
            callee_ret_bbs.push_back(bb_callee);
        }
    }
    
    return callee_ret_bbs;
}

void Inlining::MoveConstants(Graph* callee, Inst* call_inst)
{
    BasicBlock* caller_inst_bb = call_inst->GetBB();
    // move constants from callee to caller
    Inst* first_callee_inst = callee->GetBasicBlocks()[0]->GetFirstInst();
    while (first_callee_inst->GetOpcode() == Opcode::CONSTANT) {
        // unbind from callee first basic block
        first_callee_inst->GetBB()->UnbindFrontInst();
        // bind to caller first basic block
        first_callee_inst->SetNext(nullptr); // hack
        caller_inst_bb->GetGraph()->GetBasicBlocks()[0]->PushFrontInst(first_callee_inst);

        first_callee_inst = callee->GetBasicBlocks()[0]->GetFirstInst();
    }
}

void Inlining::SplitMoveAndConnectBlocks(Graph* callee, Inst* call_inst, const std::vector<BasicBlock*>& callee_ret_bbs)
{
    BasicBlock* caller_inst_bb = call_inst->GetBB();
    // split block with call instruction
    BasicBlock* call_cont_block = new BasicBlock(BasicBlock::NextId());
    // move all instructions after call inst to call_cont_block
    while(caller_inst_bb->GetLastInst() != call_inst) {
        call_cont_block->PushFrontInst(caller_inst_bb->GetLastInst());
        caller_inst_bb->UnbindBackInst();
    }

    // move callee blocks to caller
    for (auto bb: callee->GetBasicBlocks()) {
        bb->SetGraph(nullptr);
        caller_inst_bb->GetGraph()->AddBasicBlock(bb);
    }
    caller_inst_bb->GetGraph()->AddBasicBlock(call_cont_block);

    // connect blocks
    for (auto call_block_succ: caller_inst_bb->GetSuccs()) {
        call_cont_block->AddSucc(call_block_succ);
        call_block_succ->RemovePred(caller_inst_bb);
        call_block_succ->AddPred(call_cont_block);
        caller_inst_bb->RemoveSucc(call_block_succ);
    }
    caller_inst_bb->AddSucc(callee->GetBasicBlocks()[0]);
    callee->GetBasicBlocks()[0]->AddPred(caller_inst_bb);
    for (auto callee_ret_bb: callee_ret_bbs) {
        callee_ret_bb->AddSucc(call_cont_block);
        call_cont_block->AddPred(callee_ret_bb);
    }
}