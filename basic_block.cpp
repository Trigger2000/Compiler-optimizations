#include "basic_block.h"

// template <uint32_t element>
// uint32_t ElementAccessor()
// {
//     return element;
// }

// template <uint32_t... succs>
// BasicBlock* BasicBlock::BasicBlockBuilder(std::initializer_list<InstNode*> insts)
// {
//     if (insts.size() < 2) {
//         std::cerr << "invalid number of instructions in basic block" << std::endl;
//         std::abort();
//     }

//     BasicBlock *result = new BasicBlock;
//     result->first_inst = *(insts.begin());
//     result->last_inst = *(std::prev(insts.end(), 1));

//     result->first_inst->SetNext(*(std::next(insts.begin(), 1)));
//     result->last_inst->SetPrev(*(std::prev(insts.end(), 2)));
//     for (auto item = std::next(insts.begin(), 1); item < std::prev(insts.end(), 1); std::advance(item, 1))
//     {
//         (*item)->SetPrev(*(std::prev(item, 1)));
//         (*item)->SetNext(*(std::next(item, 1)));
//     }

//     return result;
// }