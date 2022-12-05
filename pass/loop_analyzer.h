#ifndef LOOP_ANALYZER_H
#define LOOP_ANALYZER_H

#include "ir/graph.h"

class Loop {
public:
    Loop(BasicBlock* back_edge_source, BasicBlock* header, bool is_reducable):
    back_edge_source_(back_edge_source), header_(header), is_reducible_(is_reducable) {}
    Loop() = default;
    ~Loop();

    bool IsReducable()
    {
        return is_reducible_;
    }

    ACCESSOR_MUTATOR(back_edge_source_, BackEdgeSource, BasicBlock*);
    ACCESSOR_MUTATOR(header_, Header, BasicBlock*);
    ACCESSOR_MUTATOR(pre_header_, PreHeader, BasicBlock*);
    ACCESSOR_MUTATOR(inner_loops_, InnerLoops, std::vector<Loop*>&);
    ACCESSOR_MUTATOR(outer_loop_, OuterLoop, Loop*);

    void PushBackBlock(BasicBlock* block)
    {
        blocks_.push_back(block);
    }

    const std::vector<BasicBlock*>& GetBlocks()
    {
        return blocks_;
    }

    void CreatePreHeader();
    void SplitLoops();

private:
    BasicBlock *back_edge_source_ = nullptr;
    BasicBlock *header_ = nullptr;
    BasicBlock *pre_header_ = nullptr;
    bool is_reducible_ = true;
    std::vector<Loop*> inner_loops_;
    Loop* outer_loop_ = nullptr;

    std::vector<BasicBlock*> blocks_;
};


class LoopAnalyzer {
public:
    void RunPassImpl(Graph *g);

private:
    void CollectBackEdges();
    void ProccessEdge(BasicBlock *root, BasicBlock *prev);
    void PopulateLoops();
    void LoopSearch(Loop *loop, BasicBlock* block);
    void BuildLoopTree();

    Graph *g_;
    marker black_marker_;
    marker gray_marker_;
};

#endif // LOOP_ANALYZER_H
