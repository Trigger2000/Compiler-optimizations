#ifndef BASIC_BLOCK_H
#define BASIC_BLOCK_H

#include <initializer_list>
#include <iostream>
#include <iterator>
#include <vector>
#include <variant>

#include "inst.h"
#include "marker.h"

class Graph;
class Loop;

class BasicBlock : public Markers
{
  public:
    BasicBlock(uint32_t bb_id) : id_(bb_id)
    {
        if (next_id_ < bb_id) {
            next_id_ = bb_id;
        }
        next_id_++;
    }
    static void BasicBlockDestroyer(BasicBlock* bb);

    void PushBackInst(Inst* inst)
    {
        assert(inst->GetPrev() == nullptr);
        inst->SetPrev(last_inst_);
        inst->SetBB(this);
        if (last_inst_ != nullptr)
            last_inst_->SetNext(inst);
        last_inst_ = inst;
        size_++;
    }

    void PushFrontInst(Inst* inst)
    {
        assert(inst->GetNext() == nullptr);
        inst->SetNext(first_inst_);
        inst->SetBB(this);
        if (first_inst_ != nullptr)
            first_inst_->SetPrev(inst);
        first_inst_ = inst;
        size_++;
    }

    // TODO fix pop instructions
    void PopFrontInst()
    {
        delete first_inst_;
        size_--;
    }

    void PopBackInst()
    {
        delete last_inst_;
        size_--;
    }

    void UnbindFrontInst()
    {
        first_inst_->GetNext()->SetPrev(nullptr);
        auto next_first = first_inst_->GetNext();
        first_inst_->SetNext(nullptr);
        first_inst_ = next_first;
        size_--;
    }

    void UnbindBackInst()
    {
        last_inst_->GetPrev()->SetNext(nullptr);
        auto next_last = last_inst_->GetPrev();
        last_inst_->SetPrev(nullptr);
        last_inst_ = next_last;
        size_--;
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
        size_++;
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
    ACCESSOR_MUTATOR(graph_, Graph, Graph*)
    ACCESSOR_MUTATOR(id_, Id, uint32_t)
    ACCESSOR_MUTATOR(size_, Size, uint32_t)
    ACCESSOR_MUTATOR(dominators_, Dominators, const std::vector<BasicBlock*>&)
    ACCESSOR_MUTATOR(idom_, IDom, BasicBlock*)
    ACCESSOR_MUTATOR(loop_, Loop, Loop*)

    const std::vector<BasicBlock*>& GetPreds() const
    {
        return preds_;
    }

    const std::vector<BasicBlock*>& GetSuccs() const
    {
        return succs_;
    }

    void AddSucc(BasicBlock* bb)
    {
        succs_.push_back(bb);
    }

    bool HasSucc(BasicBlock* bb)
    {
        for (auto& succ: succs_) {
            if (bb == succ) {
                return true;
            }
        }
        return false;
    }

    void AddPred(BasicBlock* bb)
    {
        preds_.push_back(bb);
    }

    void RemoveSucc(BasicBlock* bb)
    {
        succs_.erase(std::find(succs_.begin(), succs_.end(), bb));
    }

    void RemovePred(BasicBlock* bb)
    {
        preds_.erase(std::find(preds_.begin(), preds_.end(), bb));
    }

    void AddDominator(BasicBlock* bb)
    {
        dominators_.push_back(bb);
    }

    static uint32_t NextId()
    {
        return next_id_;
    }
  private:
    BasicBlock() = default;
    BasicBlock(BasicBlock& bb) = default;

    std::vector<BasicBlock*> preds_;
    std::vector<BasicBlock*> succs_;

    // TODO remove this
    std::vector<BasicBlock*> dominators_;
    BasicBlock* idom_ = nullptr;

    Inst* first_inst_ = nullptr;
    Inst* last_inst_ = nullptr;
    Graph* graph_ = nullptr;
    Loop* loop_ = nullptr;

    uint32_t id_ = 0;
    uint32_t size_ = 0;
    
    static inline uint32_t next_id_ = 0;
};

#endif // BASIC_BLOCK_H
