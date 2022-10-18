#include "basic_block.h"

void BasicBlock::BasicBlockDestroyer(BasicBlock* bb)
{
    assert(bb != nullptr);
    for (Inst* item = bb->GetFirstInst(); item != nullptr;) {
        Inst* curr = item;
        item = item->GetNext();
        Inst::InstDestroyer(curr);
    }

    delete bb;
}

void BasicBlock::Dump()
{
    std::cout << "bb id " << id_ << "\npreds [ ";
    for (auto item : preds) {
        std::cout << item.first << " ";
    }
    std::cout << "]\nsuccs [ ";
    for (auto item : succs) {
        std::cout << item.first << " ";
    }
    std::cout << "]\n";

    for (Inst* item = first_inst_; item != nullptr; item = item->GetNext()) {
        item->Dump();
    }
}
