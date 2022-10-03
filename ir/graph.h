#ifndef GRAPH_H
#define GRAPH_H

#include <algorithm>

#include "basic_block.h"

class Graph
{
  public:
    Graph(std::initializer_list<BasicBlock*> bbs);

    static void GraphDestroyer(Graph* graph);
    void Dump();

  private:
    std::vector<BasicBlock*> basic_blocks;

    void* method_info = nullptr;
    void* runtime_info = nullptr;
    void* optimizations_info = nullptr;
    void* backend_info = nullptr;
};

Graph::Graph(std::initializer_list<BasicBlock*> bbs) : basic_blocks(bbs)
{
    // Bind basic blocks within graph
    // TODO Maybe not efficient? O(2 * N^2)
    for (auto curr_bb : basic_blocks) {
        uint32_t curr_id = curr_bb->GetId();
        std::vector<uint32_t> predecessors;
        std::vector<BasicBlock*> predecessors_ref;
        std::vector<BasicBlock*> successors_ref;
        const std::vector<uint32_t>& curr_successors = curr_bb->GetSuccs();

        for (auto bb : basic_blocks) {
            const std::vector<uint32_t>& succs = bb->GetSuccs();
            if (std::find(succs.begin(), succs.end(), curr_id) != succs.end()) {
                predecessors.push_back(bb->GetId());
                predecessors_ref.push_back(bb);
            }
        }

        for (auto curr_succ : curr_successors) {
            // TODO It's time for std::find and lambda?
            for (auto bb : basic_blocks) {
                if (bb->GetId() == curr_succ) {
                    successors_ref.push_back(bb);
                }
            }
        }

        curr_bb->SetSuccsRef(std::move(successors_ref));
        curr_bb->SetPredsRef(std::move(predecessors_ref));
        curr_bb->SetPreds(std::move(predecessors));
    }
}

void Graph::GraphDestroyer(Graph* graph)
{
    assert(graph != nullptr);
    for (auto item : graph->basic_blocks) {
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
