#ifndef GRAPH_H
#define GRAPH_H

#include "basic_block.h"

class Graph
{
  public:
    // Create graph and bind basic blocks within it, also assign successors and predecessors
    Graph(std::initializer_list<BasicBlock*> bbs);
    ~Graph()
    {
        for (auto item : basic_blocks_) {
            BasicBlock::BasicBlockDestroyer(item);
        }
    }

    BasicBlock *GetBBbyId(uint32_t id);
    Inst* GetInstById(uint32_t id);

    ACCESSOR_MUTATOR(basic_blocks_, BasicBlocks, const std::vector<BasicBlock*>&)

    void AddBasicBlock(BasicBlock* bb)
    {
        assert(bb->GetGraph() == nullptr);
        bb->SetGraph(this);
        basic_blocks_.push_back(bb);
    }

    // not sure that these two methods should be located in Graph class
    // Add edges between bb and other bbs in graph
    void BindBasicBlock(BasicBlock* bb)
    {
        for (auto pred: bb->GetPreds()) {
            pred.second->AddSucc(bb);
        }

        for (auto succ: bb->GetSuccs()) {
            succ.second->AddPred(bb);
        }
    }

    // Remove edges between bb and other bbs in graph preserving succs and preds in bb itself
    void UnbindBasicBlock(BasicBlock* bb)
    {
        for (auto pred: bb->GetPreds()) {
            pred.second->RemoveSucc(bb);
        }

        for (auto succ: bb->GetSuccs()) {
            succ.second->RemovePred(bb);
        }
    }

    void BuildDFG();
    void BuildDominatorTreeSlow();
    std::vector<BasicBlock*> GetRPO();

    void Dump();

  private:
    std::vector<BasicBlock*> basic_blocks_;

    void* method_info = nullptr;
    void* runtime_info = nullptr;
    void* optimizations_info = nullptr;
    void* backend_info = nullptr;
};

#endif // GRAPH_H
