#ifndef GRAPH_H
#define GRAPH_H

#include <bitset>

#include "basic_block.h"
#include "marker.h"
#include "loop.h"
#include "pass/pass_manager.h"

class Graph : public PassManager, public MarkerManager
{
  public:
    Graph(std::initializer_list<BasicBlock*> bbs) : basic_blocks_(bbs) {}

    ~Graph();
    static void GraphDestroyer(Graph *g);
    
    template <typename Pass>
    void RunPass();

    template <typename Pass>
    bool IsPassValid();

    template <typename Pass>
    void SetPassValidity(bool is_valid);

    // Check that prob_dominator dominates prob_dominated
    bool CheckDominance(BasicBlock *prob_dominator, BasicBlock *prob_dominated);

    BasicBlock *GetBBbyId(uint32_t id);
    Inst* GetInstById(uint32_t id);

    ACCESSOR_MUTATOR(basic_blocks_, BasicBlocks, const std::vector<BasicBlock*>&)
    ACCESSOR_MUTATOR(rpo_basic_blocks_, RPOBasicBlocks, std::vector<BasicBlock*>)
    ACCESSOR_MUTATOR(root_loop_, RootLoop, Loop*)

    void AddBasicBlock(BasicBlock* bb);

    void Clear();

    void Dump();

  private:
    std::vector<BasicBlock*> basic_blocks_;
    std::vector<BasicBlock*> rpo_basic_blocks_;
    Loop* root_loop_ = nullptr;

    std::bitset<std::tuple_size_v<PassList>> pass_validity_;
};

template <typename Pass>
void Graph::RunPass()
{
    PassManager::RunPass<Pass>(this);
}

template <typename Pass>
bool Graph::IsPassValid()
{
    return pass_validity_[GetPassIndex<Pass>()];
}

template <typename Pass>
void Graph::SetPassValidity(bool is_valid)
{
    pass_validity_[GetPassIndex<Pass>()] = is_valid;
}

#endif // GRAPH_H
