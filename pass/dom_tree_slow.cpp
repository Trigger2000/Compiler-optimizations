#include "dom_tree_slow.h"

void DomTreeSlow::RunPassImpl(Graph* g)
{   
    basic_blocks_ = g->GetBasicBlocks();
    assert(basic_blocks_.size() > 0);
    // DFS order actually doesn't matter, let it be, for example, RPO
    g->RunPass<RPO>();
    std::vector<BasicBlock*> init_reach = g->GetRPOBasicBlocks();

    BasicBlock *root = basic_blocks_[0];
    root->AddDominator(root);

    for (auto bb = std::next(basic_blocks_.begin(), 1); bb != basic_blocks_.end(); std::advance(bb, 1)) {
        (*bb)->AddDominator(root);

        // unbind bb from graph
        (*bb)->SetGraph(nullptr);
        g->SetPassValidity<RPO>(false); // TODO remove this
        
        g->RunPass<RPO>();
        std::vector<BasicBlock*> reach_curr = g->GetRPOBasicBlocks();
        std::vector<BasicBlock*> reach_diff = ComputeVectorsDiff(init_reach, reach_curr);
        
        for (auto bb_diff: reach_diff) {
            bb_diff->AddDominator(*bb);
        }
        
        // bind bb with graph
        (*bb)->SetGraph(g);
        g->SetPassValidity<RPO>(false); // TODO remove this
    }
}
