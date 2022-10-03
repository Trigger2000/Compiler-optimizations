#ifndef BASIC_BLOCK_H
#define BASIC_BLOCK_H

#include <initializer_list>
#include <iterator>
#include <vector>
#include <iostream>

#include "inst.h"

class BasicBlock {
public:
    template <uint32_t bb_id, uint32_t... successors>
    static BasicBlock *BasicBlockBuilder(std::initializer_list<InstNode*> insts);
    static void BasicBlockDestroyer(BasicBlock *bb);

    void Dump();

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

    const std::vector<uint32_t>& GetPreds()
    {
        return preds;
    }

    const std::vector<uint32_t>& GetSuccs()
    {
        return succs;
    }

    // TODO clarify usage of rvalue refs
    void SetPreds(std::vector<uint32_t>&& predecessors)
    {
        preds = std::move(predecessors);
    }

    void SetPredsRef(std::vector<BasicBlock*>&& predecessors_ref)
    {
        preds_ref = std::move(predecessors_ref);
    }

    void SetSuccsRef(std::vector<BasicBlock*>&& successors_ref)
    {
        succs_ref = std::move(successors_ref);
    }

private:
    BasicBlock() = default;
    BasicBlock(BasicBlock &bb) = default;

    // These guys are binded in Graph constructor
    std::vector<uint32_t> preds;
    std::vector<BasicBlock*> preds_ref;
    std::vector<uint32_t> succs;
    std::vector<BasicBlock*> succs_ref;

    InstNode *first_inst = nullptr;
    InstNode *last_inst = nullptr;
    InstNode *first_phi = nullptr;
    // Graph *graph = nullptr;

    uint32_t id = 0;
};

template <uint32_t bb_id, uint32_t... successors>
BasicBlock *BasicBlock::BasicBlockBuilder(std::initializer_list<InstNode*> insts)
{
    // TODO Change, since empty bbs and bbs with 1 instruction exists!
    if (insts.size() < 2) {
        std::cerr << "invalid number of instructions in basic block" << std::endl;
        std::abort();
    }
    
    BasicBlock *result = new BasicBlock;

    // Bind instructions within basic block
    result->first_inst = *(insts.begin());
    result->last_inst = *(std::prev(insts.end(), 1));
    result->first_inst->SetNext(*(std::next(insts.begin(), 1)));
    result->last_inst->SetPrev(*(std::prev(insts.end(), 2)));
    result->first_inst->SetBBid(bb_id);
    result->last_inst->SetBBid(bb_id);
    for (auto item = std::next(insts.begin(), 1); item < std::prev(insts.end(), 1); std::advance(item, 1))
    {
        (*item)->SetBBid(bb_id);
        (*item)->SetBBid(bb_id);
        (*item)->SetPrev(*(std::prev(item, 1)));
        (*item)->SetNext(*(std::next(item, 1)));
    }

    result->id = bb_id;
    if constexpr (sizeof...(successors) != 0) {
        result->succs = std::vector{successors...};
    }

    return result;
}

void BasicBlock::BasicBlockDestroyer(BasicBlock *bb)
{
    assert(bb != nullptr);
    for (InstNode *item = bb->GetFirstinst(); item != nullptr;)
    {   
        InstNode *curr = item;
        item = item->GetNext();
        InstNode::InstDestroyer(curr);
    }

    delete bb;
}

void BasicBlock::Dump()
{
    std::cout << "bb id " << id << "\npreds [ ";
    for (auto item: preds) {
        std::cout << item << " ";
    }
    std::cout << "]\nsuccs [ ";
    for (auto item: succs) {
        std::cout << item << " ";
    }
    std::cout << "]\n";
    
    for (InstNode *item = first_inst; item != nullptr; item = item->GetNext()) {
        item->Dump();
    }
}


#endif // BASIC_BLOCK_H
