#include "loop_analyzer.h"
#include "dom_tree_fast.h"
#include "rpo.h"

Loop::~Loop()
{
    for (auto inner_loop: inner_loops_) {
        delete inner_loop;
    }
}

void LoopAnalyzer::RunPassImpl(Graph *g)
{
    g_ = g;
    g->RunPass<DomTreeFast>();
    CollectBackEdges();
    PopulateLoops();
    BuildLoopTree();
}

void LoopAnalyzer::CollectBackEdges()
{
    black_marker_ = g_->NewMarker();
    gray_marker_ = g_->NewMarker();
    ProccessEdge(g_->GetBasicBlocks()[0], nullptr);
    g_->EraseMarker(black_marker_);
    g_->EraseMarker(gray_marker_);
}

void LoopAnalyzer::ProccessEdge(BasicBlock *curr, BasicBlock *prev)
{
    if (curr->IsMarked(gray_marker_)) {
        bool is_reducable = g_->CheckDominance(curr, prev);
        if (!is_reducable) {
            return;
        }
        Loop *loop = new Loop(prev, curr, is_reducable);
        curr->SetLoop(loop);
        prev->SetLoop(loop);
        return;
    }
    if (curr->IsMarked(black_marker_)) {
        return;
    }

    curr->SetMarker(black_marker_);
    curr->SetMarker(gray_marker_);
    for (auto succ: curr->GetSuccs()) {
        ProccessEdge(std::get<BasicBlock*>(succ), curr);
    }
    curr->ResetMarker(gray_marker_);
}

void LoopAnalyzer::PopulateLoops()
{
    g_->RunPass<RPO>();
    auto rpo_bbs = g_->GetRPOBasicBlocks();
    for (auto it = rpo_bbs.rbegin(); it != rpo_bbs.rend(); it++) {
        auto header_block = *it;
        if (header_block->GetLoop() == nullptr) {
            continue;
        }
        if (header_block->GetLoop()->GetHeader() != header_block) {
            continue;
        }

        auto loop = header_block->GetLoop();
        // This check does not needed, left here to be
        // compatible with the original algorithm
        if (loop->IsReducable()) {
            black_marker_ = g_->NewMarker();
            header_block->SetMarker(black_marker_);

            BasicBlock* back_edge_source = loop->GetBackEdgeSource();
            loop->PushBackBlock(back_edge_source);
            back_edge_source->SetMarker(black_marker_);
            for (auto pred: back_edge_source->GetPreds()) {
                LoopSearch(loop, pred);
            }

            loop->PushBackBlock(header_block);
            g_->EraseMarker(black_marker_);
        }
    }
}

void LoopAnalyzer::LoopSearch(Loop *loop, BasicBlock* block)
{
    if (!block->IsMarked(black_marker_)) {
        block->SetMarker(black_marker_);
        loop->PushBackBlock(block);
        if (block->GetLoop() != nullptr && block->GetLoop()->GetOuterLoop() == nullptr) {
            loop->GetInnerLoops().push_back(block->GetLoop());
            block->GetLoop()->SetOuterLoop(loop);
        } else {
            block->SetLoop(loop);
        }
        for (auto pred: block->GetPreds()) {
            LoopSearch(loop, pred);
        }
    }
}

void LoopAnalyzer::BuildLoopTree()
{
    Loop *root_loop = new Loop();
    for (auto block: g_->GetBasicBlocks()) {
        if (block->GetLoop() == nullptr) {
            root_loop->PushBackBlock(block);
        } else if (block->GetLoop()->GetOuterLoop() == nullptr) {
            auto& inner_loops = root_loop->GetInnerLoops();
            inner_loops.push_back(block->GetLoop());
            for (auto loop_block: block->GetLoop()->GetBlocks()) {
                if (loop_block->GetLoop()->GetOuterLoop() == nullptr) {
                    loop_block->GetLoop()->SetOuterLoop(root_loop);
                }
            }
            block->GetLoop()->SetOuterLoop(root_loop);
        }
    }
    g_->SetRootLoop(root_loop);
}
