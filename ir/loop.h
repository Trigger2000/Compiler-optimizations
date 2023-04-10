#ifndef LOOP_H
#define LOOP_H

#include "basic_block.h"

class Loop {
public:
    Loop(BasicBlock* back_edge_source, BasicBlock* header, bool is_reducable):
    back_edge_source_(back_edge_source), header_(header), is_reducible_(is_reducable) {}
    Loop() = default;
    ~Loop()
    {
        for (auto inner_loop: inner_loops_) {
            delete inner_loop;
        }
    }

    bool IsReducable()
    {
        return is_reducible_;
    }

    ACCESSOR_MUTATOR(back_edge_source_, BackEdgeSource, BasicBlock*);
    ACCESSOR_MUTATOR(header_, Header, BasicBlock*);
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

private:

    BasicBlock *back_edge_source_ = nullptr;
    BasicBlock *header_ = nullptr;
    bool is_reducible_ = true;
    std::vector<Loop*> inner_loops_;
    Loop* outer_loop_ = nullptr;

    std::vector<BasicBlock*> blocks_;
};

#endif  // LOOP_H