#include "inst.h"

#define INST InstNode::InstBuilder

int main()
{
    InstNode *ins1 = INST(Opcode::ADD, 1, 2, 3);
    InstNode *ins2 = INST(Opcode::ADD, 4, 5, 6);
    InstNode *ins3 = INST(Opcode::ADD, 7, 8, 9);
    InstNode *ins4 = INST(Opcode::ADD, 10, 11, 12);
    std::cout << ins4->GetDstReg() << std::endl;
    return 0;
}
