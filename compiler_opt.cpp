#include "basic_block.h"

#define INST InstNode::InstBuilder
#define DESTROY InstNode::InstDestroyer
#define BASIC_BLOCK BasicBlock

int main()
{
    // InstNode *ins1 = INST(Opcode::ADD, 1, 2, 3);
    // InstNode *ins2 = INST(Opcode::ADD, 4, 5, 6);
    // InstNode *ins3 = INST(Opcode::ADD, 7, 8, 9);
    // InstNode *ins4 = INST(Opcode::ADD, 10, 11, 12);

    // std::cout << ins4->GetDstReg() << std::endl;

    // DESTROY(ins1);
    // DESTROY(ins2);
    // DESTROY(ins3);
    // DESTROY(ins4);

    BasicBlock tmp = BASIC_BLOCK<1, 2, 3>{
        INST(Opcode::ADD, 1, 2, 3),
        INST(Opcode::ADD, 4, 5, 6),
        INST(Opcode::ADD, 7, 8, 9),
        INST(Opcode::ADD, 10, 11, 12)
    };

    std::cout << tmp.GetFirstinst()->GetSrcReg2() << std::endl;
    std::cout << tmp.GetFirstinst()->GetNext()->GetSrcReg2() << std::endl;
    std::cout << tmp.GetLastInst()->GetPrev()->GetSrcReg2() << std::endl;
    std::cout << tmp.GetLastInst()->GetSrcReg2() << std::endl;
    return 0;
}
