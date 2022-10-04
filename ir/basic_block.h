#ifndef BASIC_BLOCK_H
#define BASIC_BLOCK_H

#include <initializer_list>
#include <iostream>
#include <iterator>
#include <vector>
#include <unordered_map>

#include "inst.h"

class BasicBlock
{
  public:
    template <uint32_t bb_id, uint32_t... successors>
    static BasicBlock* BasicBlockBuilder(std::initializer_list<IListNode*> insts);
    static void BasicBlockDestroyer(BasicBlock* bb);

    void Dump();

    IListNode* GetFirstinst() const
    {
        return first_inst;
    }

    IListNode* GetLastInst() const
    {
        return last_inst;
    }

    uint32_t GetId() const
    {
        return id;
    }

    const std::unordered_map<uint32_t, BasicBlock*>& GetPreds()
    {
        return preds;
    }

    const std::unordered_map<uint32_t, BasicBlock*>& GetSuccs()
    {
        return succs;
    }

    std::unordered_map<uint32_t, BasicBlock*>& GetPredsRef()
    {
        return preds;
    }

    std::unordered_map<uint32_t, BasicBlock*>& GetSuccsRef()
    {
        return succs;
    }

  private:
    BasicBlock() = default;
    BasicBlock(BasicBlock& bb) = default;

    // These guys are binded in Graph constructor
    std::unordered_map<uint32_t, BasicBlock*> preds;
    std::unordered_map<uint32_t, BasicBlock*> succs;

    IListNode* first_inst = nullptr;
    IListNode* last_inst = nullptr;

    // TODO currently not initialized
    IListNode* first_phi = nullptr;
    // Graph *graph = nullptr;

    uint32_t id = 0;
};

template <uint32_t bb_id, uint32_t... successors>
BasicBlock* BasicBlock::BasicBlockBuilder(std::initializer_list<IListNode*> insts)
{
    // TODO Change, since empty bbs and bbs with 1 instruction exists!
    if (insts.size() < 2) {
        std::cerr << "invalid number of instructions in basic block" << std::endl;
        std::abort();
    }

    BasicBlock* result = new BasicBlock;

    // Bind instructions within basic block
    result->id = bb_id;
    result->first_inst = *(insts.begin());
    result->last_inst = *(std::prev(insts.end(), 1));
    result->first_inst->SetNext(*(std::next(insts.begin(), 1)));
    result->last_inst->SetPrev(*(std::prev(insts.end(), 2)));
    result->first_inst->SetBBid(bb_id);
    result->last_inst->SetBBid(bb_id);
    for (auto item = std::next(insts.begin(), 1); item < std::prev(insts.end(), 1); std::advance(item, 1)) {
        (*item)->SetBBid(bb_id);
        (*item)->SetBBid(bb_id);
        (*item)->SetPrev(*(std::prev(item, 1)));
        (*item)->SetNext(*(std::next(item, 1)));
    }
    
    if constexpr (sizeof...(successors) != 0) {
        // TODO remove this vector?
        std::vector<uint32_t> tmp_vector = std::vector{ successors... };
        for (auto item: tmp_vector) {
            result->succs[item] = nullptr;
        }
    }

    return result;
}

void BasicBlock::BasicBlockDestroyer(BasicBlock* bb)
{
    assert(bb != nullptr);
    for (IListNode* item = bb->GetFirstinst(); item != nullptr;) {
        IListNode* curr = item;
        item = item->GetNext();
        IListNode::InstDestroyer(curr);
    }

    delete bb;
}

void BasicBlock::Dump()
{
    std::cout << "bb id " << id << "\npreds [ ";
    for (auto item : preds) {
        std::cout << item.first << " ";
    }
    std::cout << "]\nsuccs [ ";
    for (auto item : succs) {
        std::cout << item.first << " ";
    }
    std::cout << "]\n";

    for (IListNode* item = first_inst; item != nullptr; item = item->GetNext()) {
        item->Dump();
    }
}

#endif // BASIC_BLOCK_H
