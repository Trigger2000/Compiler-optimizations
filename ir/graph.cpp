#include <algorithm>

#include "graph.h"

Graph::Graph(std::initializer_list<BasicBlock*> bbs) : basic_blocks_(bbs)
{
    // TODO Maybe not efficient? O(N^2), N - number of bbs
    for (auto curr_bb : basic_blocks_) {
        for (auto bb : basic_blocks_) {
            if (bb->HasSucc(curr_bb)) {
                curr_bb->AddPred(bb);
            }
            if (curr_bb->HasSucc(bb)) {
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
