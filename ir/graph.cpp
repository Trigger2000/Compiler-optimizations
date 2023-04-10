#include <algorithm>

#include "graph.h"

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

Graph::~Graph()
{
    delete root_loop_;
}

void Graph::GraphDestroyer(Graph *g)
{
    for (auto item : g->basic_blocks_) {
        BasicBlock::BasicBlockDestroyer(item);
    }
    delete g->root_loop_;
    delete g;
}

bool Graph::CheckDominance(BasicBlock *prob_dominator, BasicBlock *prob_dominated)
{
    BasicBlock* idom = prob_dominated->GetIDom();
    while (idom != nullptr) {
        if (idom == prob_dominator)
            return true;
        idom = idom->GetIDom();
    }
    return false;
}

void Graph::AddBasicBlock(BasicBlock* bb)
{
    assert(bb->GetGraph() == nullptr);
    bb->SetGraph(this);
    basic_blocks_.push_back(bb);
}

void Graph::Clear()
{
    basic_blocks_.clear();
}
