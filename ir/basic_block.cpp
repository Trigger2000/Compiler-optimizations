#include "basic_block.h"
#include "loop.h"

void BasicBlock::BasicBlockDestroyer(BasicBlock* bb)
{
    assert(bb != nullptr);
    for (Inst* item = bb->GetFirstInst(); item != nullptr;) {
        Inst* curr = item;
        item = item->GetNext();
        delete curr;
    }

    delete bb;
}

void BasicBlock::Dump()
{
    std::cout << "bb id " << id_ << "\npreds [ ";
    for (auto pred : preds_) {
        std::cout << pred->GetId() << " ";
    }
    std::cout << "]\nsuccs [ ";
    for (auto succ : succs_) {
        std::cout << succ->GetId() << " ";
    }
    std::cout << "]\ndominators [ ";
    for (auto dom : dominators_) {
        std::cout << dom->GetId() << " ";
    }
    std::cout << "]\n";

    for (Inst* item = first_inst_; item != nullptr; item = item->GetNext()) {
        item->Dump();
    }
}

bool BasicBlock::IsLoopHeader()
{
    return loop_ != nullptr && loop_->GetHeader() == this;
}