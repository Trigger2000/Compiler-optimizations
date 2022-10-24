#include <algorithm>

#include "graph.h"

Graph::Graph(std::initializer_list<BasicBlock*> bbs) : basic_blocks_(bbs)
{
    // TODO Maybe not efficient? O(N^2), N - number of bbs
    for (auto curr_bb : basic_blocks_) {
        for (auto bb : basic_blocks_) {
            if (bb->GetSuccs().find(curr_bb->GetId()) != bb->GetSuccs().end()) {
                curr_bb->AddPred(bb);
            }
            if (curr_bb->GetSuccs().find(bb->GetId()) != curr_bb->GetSuccs().end()) {
                curr_bb->AddSucc(bb);
            }
        }
        curr_bb->SetGraph(this);
    }

    BuildDFG();
}

// bad complexity?.. O(N^2), N - number of instructions in graph
void Graph::BuildDFG()
{
    for (auto bb: basic_blocks_) {
        for (auto inst = bb->GetFirstInst(); inst != nullptr; inst = inst->GetNext()) {
            // bind inputs and assign users
            if (inst->HasInput1()) {
                uint32_t input_id = inst->GetInput1()->GetInputId();
                inst->GetInput1()->SetInputInst(GetInstById(input_id));
                GetInstById(input_id)->GetUsers().AddUser(inst);
            }
            if (inst->HasInput2()) {
                uint32_t input_id = inst->GetInput2()->GetInputId();
                inst->GetInput2()->SetInputInst(GetInstById(input_id));
                GetInstById(input_id)->GetUsers().AddUser(inst);
            }
            if (inst->HasTargetInst()) {
                uint32_t input_id = inst->GetTargetInst()->GetInputId();
                inst->GetTargetInst()->SetInputInst(GetInstById(input_id));
                GetInstById(input_id)->GetUsers().AddUser(inst);
            }
            if (inst->HasPhiInputs()) {
                for (auto phi_input : inst->GetPhiInputs()) {
                    phi_input->SetInputBB(GetBBbyId(phi_input->GetInputBBId()));
                    phi_input->SetInputInst(GetInstById(phi_input->GetInputId()));
                    GetInstById(phi_input->GetInputId())->GetUsers().AddUser(inst);
                }
            }
        }
    }
}

void POVisitor(std::vector<BasicBlock*>& result, BasicBlock* current)
{
    if (current->GetDFSMarker() == true) {
        return;
    }
    current->SetDFSMarker(true);
    for (auto item: current->GetSuccs()) {
        POVisitor(result, item.second);
    }
    result.push_back(current);
}

std::vector<BasicBlock*> Graph::GetRPO()
{
    std::vector<BasicBlock*> result;
    assert(basic_blocks_.size() > 0);

    basic_blocks_[0]->SetDFSMarker(true);
    for (auto item: basic_blocks_[0]->GetSuccs()) {
        POVisitor(result, item.second);
    }
    result.push_back(basic_blocks_[0]);
    std::reverse(result.begin(), result.end());

    for (auto bb: basic_blocks_) {
        bb->SetDFSMarker(false);
    }

    return result;
}

void Graph::BuildDominatorTreeSlow()
{
    assert(basic_blocks_.size() > 0);
    // DFS order actually doesn't matter, let it be, for example, RPO
    std::vector<BasicBlock*> init_reach = GetRPO();

    BasicBlock *root = basic_blocks_[0];
    root->AddDominator(root);

    for (auto bb = std::next(basic_blocks_.begin(), 1); bb != basic_blocks_.end(); std::advance(bb, 1)) {
        (*bb)->AddDominator(root);

        UnbindBasicBlock(*bb);
        
        // DFS order actually doesn't matter, let it be, for example, RPO
        std::vector<BasicBlock*> reach_curr = GetRPO();
        std::vector<BasicBlock*> reach_diff = ComputeVectorsDiff(init_reach, reach_curr);
        
        for (auto bb_diff: reach_diff) {
            bb_diff->AddDominator(*bb);
        }

        BindBasicBlock(*bb);
    }
}

void Graph::Dump()
{
    for (auto item : basic_blocks_) {
        item->Dump();
        std::cout << "\n";
    }
}


BasicBlock* Graph::GetBBbyId(uint32_t id)
{
    for (auto item: basic_blocks_) {
        if (item->GetId() == id)
            return item;
    }
    return nullptr;
}
Inst* Graph::GetInstById(uint32_t id)
{
    Inst* result = nullptr;
    for (auto item: basic_blocks_) {
        result = item->GetInstById(id);
        if (result != nullptr) {
            return result;
        }
    }
    return result;
}
