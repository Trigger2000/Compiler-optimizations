#include "rpo.h"

void RPO::RunPassImpl(Graph *g)
{   
    basic_blocks_ = g->GetBasicBlocks();
    std::vector<BasicBlock*> result;
    basic_blocks_[0]->SetMarker(BB_VISITED_MARKER, true);

    for (auto succ: basic_blocks_[0]->GetSuccs()) {
        PostOrderVisitor(result, std::get<BasicBlock*>(succ));
    }
    result.push_back(basic_blocks_[0]);

    std::reverse(result.begin(), result.end());
    for (auto bb: basic_blocks_) {
        bb->ClearMarkers();
    }
    
    g->SetRPOBasicBlocks(result);
    g->SetMarker(PassType::RPOType, true);
}