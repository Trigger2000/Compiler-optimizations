#ifndef GRAPH_H
#define GRAPH_H

#include "basic_block.h"

class Graph
{
  public:
    // Create graph and bind basic blocks within it, also assign successors and predecessors
    // TODO DFG build has not supported yet. Will be added here
    Graph(std::initializer_list<BasicBlock*> bbs);
    ~Graph();
    void Dump();

    BasicBlock *GetBBbyId(uint32_t id)
    {
        // TODO introduce cache?
        for (auto item: basic_blocks) {
            if (item->GetId() == id)
                return item;
        }
        return nullptr;
    }

  private:
    std::vector<BasicBlock*> basic_blocks;

    void* method_info = nullptr;
    void* runtime_info = nullptr;
    void* optimizations_info = nullptr;
    void* backend_info = nullptr;
};

Graph::Graph(std::initializer_list<BasicBlock*> bbs) : basic_blocks(bbs)
{
    // TODO Maybe not efficient? O(N^2)
    for (auto curr_bb : basic_blocks) {
        uint32_t curr_id = curr_bb->GetId();
        std::unordered_map<uint32_t, BasicBlock*>& curr_predecessors = curr_bb->GetPredsRef();
        std::unordered_map<uint32_t, BasicBlock*>& curr_successors = curr_bb->GetSuccsRef();

        for (auto bb : basic_blocks) {
            const std::unordered_map<uint32_t, BasicBlock*>& succs = bb->GetSuccs();
            if (succs.find(curr_id) != succs.end()) {
                curr_predecessors[bb->GetId()] = bb;
            }
            if (curr_successors.find(bb->GetId()) != curr_successors.end()) {
                curr_successors[bb->GetId()] = bb;
            }
        }
    }
}

Graph::~Graph()
{
    for (auto item : basic_blocks) {
        BasicBlock::BasicBlockDestroyer(item);
    }
}

void Graph::Dump()
{
    for (auto item : basic_blocks) {
        item->Dump();
        std::cout << "\n";
    }
}

#endif // GRAPH_H
