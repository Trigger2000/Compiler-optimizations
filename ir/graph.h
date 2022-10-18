#ifndef GRAPH_H
#define GRAPH_H

#include "basic_block.h"

class Graph
{
  public:
    // Create graph and bind basic blocks within it, also assign successors and predecessors
    // TODO add insert/remove instructions
    Graph(std::initializer_list<BasicBlock*> bbs);
    ~Graph()
    {
        for (auto item : basic_blocks) {
            BasicBlock::BasicBlockDestroyer(item);
        }
    }

    void Dump();

    BasicBlock *GetBBbyId(uint32_t id)
    {
        for (auto item: basic_blocks) {
            if (item->GetId() == id)
                return item;
        }
        return nullptr;
    }

    Inst* GetInstById(uint32_t id)
    {
        Inst* result = nullptr;
        for (auto item: basic_blocks) {
            result = item->GetInstById(id);
            if (result != nullptr) {
                return result;
            }
        }
        return result;
    }

    std::vector<BasicBlock*> GetRPO();

  private:
    std::vector<BasicBlock*> basic_blocks;

    void* method_info = nullptr;
    void* runtime_info = nullptr;
    void* optimizations_info = nullptr;
    void* backend_info = nullptr;
};

Graph::Graph(std::initializer_list<BasicBlock*> bbs) : basic_blocks(bbs)
{
    // TODO Maybe not efficient? O(N^2), N - number of bbs
    for (auto curr_bb : basic_blocks) {
        uint32_t curr_id = curr_bb->GetId();
        std::unordered_map<uint32_t, BasicBlock*>& curr_predecessors = curr_bb->GetPreds();
        std::unordered_map<uint32_t, BasicBlock*>& curr_successors = curr_bb->GetSuccs();

        for (auto bb : basic_blocks) {
            const std::unordered_map<uint32_t, BasicBlock*>& succs = bb->GetSuccs();
            if (succs.find(curr_id) != succs.end()) {
                curr_predecessors[bb->GetId()] = bb;
            }
            if (curr_successors.find(bb->GetId()) != curr_successors.end()) {
                curr_successors[bb->GetId()] = bb;
            }
        }
        curr_bb->SetGraph(this);
    }

    // build DFG
    // bad complexity?.. O(N^2), N - number of instructions in graph
    for (auto bb: basic_blocks) {
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

void visitor(std::vector<BasicBlock*>& result, BasicBlock* current)
{
    for (auto item: current->GetSuccs()) {
        visitor(result, item.second);
    }
    result.push_back(current);
}

// TODO Didn't checked this
std::vector<BasicBlock*> Graph::GetRPO()
{
    std::vector<BasicBlock*> result;
    assert(basic_blocks.size() > 0);
    for (auto item: basic_blocks[0]->GetSuccs()) {
        visitor(result, item.second);
    }
    result.push_back(basic_blocks[0]);
    return result;
}

void Graph::Dump()
{
    for (auto item : basic_blocks) {
        item->Dump();
        std::cout << "\n";
    }
}

#endif // GRAPH_H
