#include "graph.h"

#define INST InstNode::InstBuilder
#define BASIC_BLOCK BasicBlock

int main()
{
    // InstNode *ins1 = INST(Opcode::ADD, 1, 2, 3);
    // InstNode *ins2 = INST(Opcode::ADD, 4, 5, 6);
    // InstNode *ins3 = INST(Opcode::ADD, 7, 8, 9);
    // InstNode *ins4 = INST(Opcode::ADD, 10, 11, 12);

    BasicBlock tmp = BASIC_BLOCK<1, 2, 3>{
        INST<1>(Opcode::ADD, 1, 2, 3),
        INST<2>(Opcode::ADD, 4, 5, 6),
        INST<3>(Opcode::ADD, 7, 8, 9),
        INST<4>(Opcode::ADD, 10, 11, 12)
    };

    tmp.Dump();
    tmp.Clear();
    return 0;
}
