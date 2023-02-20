#ifndef GRAPH_H
#define GRAPH_H

#include <bitset>

#include "basic_block.h"
#include "marker.h"
#include "../pass/pass_manager.h"

class Loop;

class Graph : public PassManager, public MarkerManager
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
        delete root_loop_;
    }
    
    template <typename Pass>
    void RunPass()
    {
        PassManager::RunPass<Pass>(this);
    }

    template <typename Pass>
    bool IsPassValid()
    {
        return pass_validity_[GetPassIndex<Pass>()];
    }

    template <typename Pass>
    void SetPassValidity(bool is_valid)
    {
        pass_validity_[GetPassIndex<Pass>()] = is_valid;
    }

    // Check that prob_dominator dominates prob_dominated
    bool CheckDominance(BasicBlock *prob_dominator, BasicBlock *prob_dominated)
    {
        BasicBlock* idom = prob_dominated->GetIDom();
        while (idom != nullptr) {
            if (idom == prob_dominator)
                return true;
            idom = idom->GetIDom();
        }
        return false;
    }

    BasicBlock *GetBBbyId(uint32_t id);
    Inst* GetInstById(uint32_t id);

    ACCESSOR_MUTATOR(basic_blocks_, BasicBlocks, const std::vector<BasicBlock*>&)
    ACCESSOR_MUTATOR(rpo_basic_blocks_, RPOBasicBlocks, std::vector<BasicBlock*>)
    ACCESSOR_MUTATOR(root_loop_, RootLoop, Loop*)
    ACCESSOR_MUTATOR(name_, Name, std::string)

    void AddBasicBlock(BasicBlock* bb)
    {
        assert(bb->GetGraph() == nullptr);
        bb->SetGraph(this);
        basic_blocks_.push_back(bb);
    }

    void BuildDFG();

    void Dump();

  private:
    std::string name_;
    std::vector<BasicBlock*> basic_blocks_;
    std::vector<BasicBlock*> rpo_basic_blocks_;
    Loop* root_loop_ = nullptr;

    std::bitset<std::tuple_size_v<PassList>> pass_validity_;
};

#endif // GRAPH_H
