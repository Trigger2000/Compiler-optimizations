#ifndef BASIC_BLOCK_H
#define BASIC_BLOCK_H

#include <initializer_list>
#include <iterator>
#include <vector>
#include <iostream>

#include "graph.h"
#include "inst.h"

class InstNode;

template <uint32_t bb_id, uint32_t... successors>
class BasicBlock {
public:
    BasicBlock(std::initializer_list<InstNode*> insts);

    InstNode *GetFirstinst() const
    {
        return first_inst;
    }

    InstNode *GetLastInst() const
    {
        return last_inst;
    }

    uint32_t GetId() const
    {
        return id;
    }

private:
    BasicBlock() = default;
    BasicBlock(BasicBlock &bb) = default;

    std::vector<uint32_t> preds;
    std::vector<uint32_t> succs;

    InstNode *first_inst = nullptr;
    InstNode *last_inst = nullptr;
    InstNode *first_phi = nullptr;
    Graph *graph = nullptr;

    uint32_t id = 0;
};

template <uint32_t bb_id, uint32_t... successors>
BasicBlock<bb_id, successors...>::BasicBlock(std::initializer_list<InstNode*> insts)
{
    if (insts.size() < 2) {
        std::cerr << "invalid number of instructions in basic block" << std::endl;
        std::abort();
    }

    first_inst = *(insts.begin());
    last_inst = *(std::prev(insts.end(), 1));
    first_inst->SetNext(*(std::next(insts.begin(), 1)));
    last_inst->SetPrev(*(std::prev(insts.end(), 2)));
    for (auto item = std::next(insts.begin(), 1); item < std::prev(insts.end(), 1); std::advance(item, 1))
    {
        (*item)->SetPrev(*(std::prev(item, 1)));
        (*item)->SetNext(*(std::next(item, 1)));
    }

    id = bb_id;
    succs = std::vector{successors...};
}

#endif // BASIC_BLOCK_H
