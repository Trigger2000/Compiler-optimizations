#ifndef GRAPH_H
#define GRAPH_H

#include "basic_block.h"
#include "marker.h"
#include "../pass/pass_manager.h"

class Graph : public Markers
{
  public:
    // Create graph and bind basic blocks within it, also assign successors and predecessors
    Graph(std::initializer_list<BasicBlock*> bbs);
    Graph() = delete;
    ~Graph()
    {
        for (auto item : basic_blocks_) {
            BasicBlock::BasicBlockDestroyer(item);
        }
    }
    
    template <typename Pass>
    void RunPass()
    {
        pm_.RunPass<Pass>(this);
    }

    BasicBlock *GetBBbyId(uint32_t id);
    Inst* GetInstById(uint32_t id);

    ACCESSOR_MUTATOR(basic_blocks_, BasicBlocks, const std::vector<BasicBlock*>&)
    ACCESSOR_MUTATOR(rpo_basic_blocks_, RPOBasicBlocks, std::vector<BasicBlock*>)

    void AddBasicBlock(BasicBlock* bb)
    {
        assert(bb->GetGraph() == nullptr);
        bb->SetGraph(this);
        basic_blocks_.push_back(bb);
    }

    void BuildDFG();

    void Dump();

  private:
    std::vector<BasicBlock*> basic_blocks_;
    std::vector<BasicBlock*> rpo_basic_blocks_;

    PassManager pm_;
};

#endif // GRAPH_H
