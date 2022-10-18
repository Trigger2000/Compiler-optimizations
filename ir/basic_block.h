#ifndef BASIC_BLOCK_H
#define BASIC_BLOCK_H

#include <initializer_list>
#include <iostream>
#include <iterator>
#include <vector>
#include <unordered_map>

#include "inst.h"

class Graph;

class BasicBlock
{
  public:
    template <uint32_t... successors>
    static BasicBlock* BasicBlockBuilder(uint32_t bb_id, std::initializer_list<Inst*> insts);
    static void BasicBlockDestroyer(BasicBlock* bb);

    void Dump();

    ACCESSOR_MUTATOR(first_inst_, FirstInst, Inst*)
    ACCESSOR_MUTATOR(last_inst_, LastInst, Inst*)
    ACCESSOR_MUTATOR(first_phi_, FirstPhi, Inst*)
    ACCESSOR_MUTATOR(graph_, Graph, Graph*)
    ACCESSOR_MUTATOR(id_, Id, uint32_t)

    const std::unordered_map<uint32_t, BasicBlock*>& GetPreds() const
    {
        return preds;
    }

    const std::unordered_map<uint32_t, BasicBlock*>& GetSuccs() const
    {
        return succs;
    }

    std::unordered_map<uint32_t, BasicBlock*>& GetPreds()
    {
        return preds;
    }

    std::unordered_map<uint32_t, BasicBlock*>& GetSuccs()
    {
        return succs;
    }

  private:
    BasicBlock() = default;
    BasicBlock(BasicBlock& bb) = default;

    // These guys are binded in Graph constructor
    // TODO Remove uint32_t id and change to list/vector
    std::unordered_map<uint32_t, BasicBlock*> preds;
    std::unordered_map<uint32_t, BasicBlock*> succs;

    Inst* first_inst_ = nullptr;
    Inst* last_inst_ = nullptr;
    Inst* first_phi_ = nullptr;
    Graph* graph_ = nullptr;

    uint32_t id_ = 0;
};

template <uint32_t... successors>
BasicBlock* BasicBlock::BasicBlockBuilder(uint32_t bb_id, std::initializer_list<Inst*> insts)
{
    BasicBlock* result = new BasicBlock;

    // Bind instructions within basic block
    result->id_ = bb_id;
    if (insts.size() > 0) {
        result->first_inst_ = *(insts.begin());
        result->last_inst_ = *(std::prev(insts.end(), 1));
        result->first_inst_->SetBB(result);
        result->last_inst_->SetBB(result);
    }
    if (insts.size() > 1) {
        result->first_inst_->SetNext(*(std::next(insts.begin(), 1)));
        result->last_inst_->SetPrev(*(std::prev(insts.end(), 2)));
    }
    for (auto item = std::next(insts.begin(), 1); item < std::prev(insts.end(), 1); std::advance(item, 1)) {
        (*item)->SetBB(result);
        (*item)->SetBB(result);
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

#endif // BASIC_BLOCK_H
