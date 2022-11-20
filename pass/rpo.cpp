#include "rpo.h"
#include "../ir/marker.h"

void RPO::RunPassImpl(Graph *g)
{   
    basic_blocks_ = g->GetBasicBlocks();
    std::vector<BasicBlock*> result;

    marker visited_marker = g->NewMarker();
    basic_blocks_[0]->SetMarker(visited_marker);

    for (auto succ: basic_blocks_[0]->GetSuccs()) {
        PostOrderVisitor(result, std::get<BasicBlock*>(succ), visited_marker);
    }
    result.push_back(basic_blocks_[0]);

    std::reverse(result.begin(), result.end());
    
    g->SetRPOBasicBlocks(result);
    g->EraseMarker(visited_marker);
}

void RPO::PostOrderVisitor(std::vector<BasicBlock*>& result, BasicBlock* current, marker visited_marker)
{
    if (current->IsMarked(visited_marker) || /* presence */ current->GetGraph() == nullptr) {
        return;
    }
    current->SetMarker(visited_marker);
    for (auto succ: current->GetSuccs()) {
        PostOrderVisitor(result, std::get<BasicBlock*>(succ), visited_marker);
    }
    result.push_back(current);
}