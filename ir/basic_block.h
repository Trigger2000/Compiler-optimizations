#ifndef BASIC_BLOCK_H
#define BASIC_BLOCK_H

#include <initializer_list>
#include <iostream>
#include <iterator>
#include <vector>
#include <unordered_map>
#include <set>

#include "inst.h"

class Graph;

class BasicBlock
{
  public:
    // TODO remove id from template parameters
    template <uint32_t bb_id, uint32_t... successors>
    static BasicBlock* BasicBlockBuilder(std::initializer_list<Inst*> insts);
    static void BasicBlockDestroyer(BasicBlock* bb);

    void PushBackInst(Inst* inst)
    {
        assert(inst->GetPrev() == nullptr);
        inst->SetPrev(last_inst_);
        inst->SetBB(this);
        last_inst_ = inst;
    }

    void PushFrontInst(Inst* inst)
    {
        assert(inst->GetNext() == nullptr);
        inst->SetNext(inst);
        inst->SetBB(this);
        first_inst_ = inst;
    }

    // inserts inst before reference_inst
    void InsertInst(Inst* reference_inst, Inst* inst)
    {
        assert(inst->GetPrev() == nullptr);
        assert(inst->GetNext() == nullptr);
        inst->SetPrev(reference_inst->GetPrev());
        reference_inst->GetPrev()->SetNext(inst);
        inst->SetNext(reference_inst);
        reference_inst->SetPrev(inst);
        inst->SetBB(this);
    }

    bool IsFirstBB()
    {
        assert(graph_ != nullptr);
        return preds_.size() == 0;
    }

    bool IsLastBB()
    {
        assert(graph_ != nullptr);
        return succs_.size() == 0;
    }

    Inst* GetInstById(uint32_t id)
    {
        for (auto item = first_inst_; item != nullptr; item = item->GetNext()) {
            if (item->GetId() == id) {
                return item;
            } 
        }
        return nullptr;
    }

    void Dump();

    ACCESSOR_MUTATOR(first_inst_, FirstInst, Inst*)
    ACCESSOR_MUTATOR(last_inst_, LastInst, Inst*)
    ACCESSOR_MUTATOR(first_phi_, FirstPhi, Inst*)
    ACCESSOR_MUTATOR(graph_, Graph, Graph*)
    ACCESSOR_MUTATOR(id_, Id, uint32_t)
    ACCESSOR_MUTATOR(dfs_marker_, DFSMarker, bool)
    ACCESSOR_MUTATOR(dominators_, Dominators, const std::vector<BasicBlock*>&)

    const std::unordered_map<uint32_t, BasicBlock*>& GetPreds() const
    {
        return preds_;
    }

    const std::unordered_map<uint32_t, BasicBlock*>& GetSuccs() const
    {
        return succs_;
    }

    void AddSucc(BasicBlock* bb)
    {
        succs_[bb->GetId()] = bb;
    }

    void AddPred(BasicBlock* bb)
    {
        preds_[bb->GetId()] = bb;
    }

    void RemoveSucc(BasicBlock* bb)
    {
        succs_.erase(bb->GetId());
    }

    void RemovePred(BasicBlock* bb)
    {
        preds_.erase(bb->GetId());
    }

    void AddDominator(BasicBlock* bb)
    {
        dominators_.push_back(bb);
    }

  private:
    BasicBlock() = default;
    BasicBlock(BasicBlock& bb) = default;

    // These guys are binded in Graph constructor
    // TODO Remove uint32_t id and change to list/vector
    std::unordered_map<uint32_t, BasicBlock*> preds_;
    std::unordered_map<uint32_t, BasicBlock*> succs_;

    std::vector<BasicBlock*> dominators_;

    Inst* first_inst_ = nullptr;
    Inst* last_inst_ = nullptr;
    Inst* first_phi_ = nullptr;
    Graph* graph_ = nullptr;

    bool dfs_marker_ = false;

    uint32_t id_ = 0;
};

template <uint32_t bb_id, uint32_t... successors>
BasicBlock* BasicBlock::BasicBlockBuilder(std::initializer_list<Inst*> insts)
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

        for (auto item = std::next(insts.begin(), 1); item < std::prev(insts.end(), 1); std::advance(item, 1)) {
            (*item)->SetBB(result);
            (*item)->SetBB(result);
            (*item)->SetPrev(*(std::prev(item, 1)));
            (*item)->SetNext(*(std::next(item, 1)));
        }
    }

    // set first_phi_
    for (auto item = insts.begin(); item < insts.end(); std::advance(item, 1)) {
        if ((*item)->GetType() == Type::InstPhi) {
            result->first_phi_ = *item;
            break;
        }
    }
    
    if constexpr (sizeof...(successors) != 0) {
        // TODO remove this vector?
        std::vector<uint32_t> tmp_vector = std::vector{ successors... };
        for (auto item: tmp_vector) {
            result->succs_[item] = nullptr;
        }
    }

    return result;
}

#endif // BASIC_BLOCK_H
