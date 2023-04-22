#include "liveness_analysis.h"
#include "linear_order.h"

void LivenessAnalysis::RunPassImpl(Graph* g)
{
    g->RunPass<LinearOrder>();
    linear_order_ = g->GetLinearOrder();

    InitLiveness();
    CalculateLifeIntervals(g);
}

void LivenessAnalysis::InitLiveness()
{
    uint32_t cur_live_num = 0;
    uint32_t cur_lin_num = 0;
    for (auto bb: linear_order_) {
        uint32_t bb_live_interval_start = cur_live_num;
        for (auto inst = bb->GetFirstInst(); inst != nullptr; inst = inst->GetNext()) {
            if (inst->GetType() != Type::InstPhi) {
                cur_live_num += 2;
            }
            inst->SetLiveNumber(cur_live_num);
            inst->SetLinearNumber(cur_lin_num);
            cur_lin_num++;
        }
        cur_live_num += 2;
        bb_live_interval_[bb] = {bb_live_interval_start, cur_live_num};
    }
}

void LivenessAnalysis::CalculateLifeIntervals(Graph *g)
{
    for (auto bb = linear_order_.rbegin(); bb != linear_order_.rend(); bb++) {
        LiveSet live_set;
        // calculate initial liveset
        for (auto succ: (*bb)->GetSuccs()) {
            live_set.Union(live_inputs_[succ]);
            
            AddPhiInputsToLiveset(*bb, succ, live_set);
        }
        
        // process initial liveset
        for (auto inst: live_set.GetLiveSet()) {
            AddInstLiveInterval(inst, bb_live_interval_[*bb].GetStart(), bb_live_interval_[*bb].GetEnd());
        }
        
        // iterate over instructions
        for (Inst* inst = (*bb)->GetLastInst(); inst != nullptr; inst = inst->GetPrev()) {
            live_set.RemoveInst(inst);
            
            if (inst->GetType() == Type::InstPhi) {
                continue;
            }

            if (inst_live_interval_.count(inst) == 0) {
                inst_live_interval_[inst] = new LiveInterval{0, inst->GetLiveNumber() + 2};
            }
            inst_live_interval_[inst]->SetStart(inst->GetLiveNumber());

            IterateOverInputs(inst, live_set);
        }

        // remove phis from liveset
        Inst* curr_inst = (*bb)->GetFirstInst();
        while (curr_inst->GetType() == Type::InstPhi) {
            live_set.RemoveInst(curr_inst);
            curr_inst = curr_inst->GetNext();
        }

        if ((*bb)->IsLoopHeader()) {
            for (auto inst: live_set.GetLiveSet()) {
                AddInstLiveInterval(inst, bb_live_interval_[*bb].GetStart(),
                                    bb_live_interval_[(*bb)->GetLoop()->GetBackEdgeSource()].GetEnd());
            }
        }

        live_inputs_[*bb] = live_set;
    }

    for (auto item: inst_live_interval_) {
        if (item.first->GetType() == Type::InstJmp || item.first->GetOpcode() == Opcode::RET_VOID ||
            item.first->GetOpcode() == Opcode::CMP) {
            inst_live_interval_[item.first]->SetStart(0);
            inst_live_interval_[item.first]->SetEnd(0);
        }
    }

    g->SetLiveIntervals(inst_live_interval_);
}

void LivenessAnalysis::AddPhiInputsToLiveset(BasicBlock *curr_bb, BasicBlock *succ, LiveSet& live_set)
{
    Inst* curr_inst = succ->GetFirstInst();
    while (curr_inst->GetType() == Type::InstPhi) {
        for (auto input: curr_inst->CastToInstPhi()->GetInputInst()) {
            if (curr_bb->HasInst(input)) {
                live_set.AddInst(input);
            }
        }
        curr_inst = curr_inst->GetNext();
    }
}

void LivenessAnalysis::IterateOverInputs(Inst* inst, LiveSet& live_set)
{
    switch (inst->GetType())
    {
        case Type::InstWithOneInput:
        {
            auto inst_casted = inst->CastToInstWithOneInput();
            live_set.AddInst(inst_casted->GetInput1());
            AddInstLiveInterval(inst_casted->GetInput1(), bb_live_interval_[inst_casted->GetBB()].GetStart(),
                                inst_casted->GetLiveNumber());
            break;
        }
        case Type::InstWithTwoInputs:
        {
            auto inst_casted = inst->CastToInstWithTwoInputs();
            live_set.AddInst(inst_casted->GetInput1());
            live_set.AddInst(inst_casted->GetInput2());
            AddInstLiveInterval(inst_casted->GetInput1(), bb_live_interval_[inst_casted->GetBB()].GetStart(),
                                inst_casted->GetLiveNumber());
            AddInstLiveInterval(inst_casted->GetInput2(), bb_live_interval_[inst_casted->GetBB()].GetStart(),
                                inst_casted->GetLiveNumber());
            break;
        }
        case Type::InstCall:
        {
            auto inst_casted = inst->CastToInstCall();
            for (auto input: inst_casted->GetArguments()) {
                live_set.AddInst(input);
                AddInstLiveInterval(input, bb_live_interval_[inst_casted->GetBB()].GetStart(),
                                    inst_casted->GetLiveNumber());
            }
            break;
        }
        case Type::InstPhi:
        {
            auto inst_casted = inst->CastToInstPhi();
            for (auto input: inst_casted->GetInputInst()) {
                live_set.AddInst(input);
                AddInstLiveInterval(input, bb_live_interval_[inst_casted->GetBB()].GetStart(),
                                    inst_casted->GetLiveNumber());
            }
            break;
        }
        default:
            break;
    }
}

void LivenessAnalysis::AddInstLiveInterval(Inst* inst, uint32_t start, uint32_t end)
{
    if (inst_live_interval_.count(inst) == 0) {
        inst_live_interval_[inst] = new LiveInterval{start, end};
    } else {
        inst_live_interval_[inst]->AddInterval(start, end);
    }
}