#include "tests/graph_test.h"

#define INST Inst::InstBuilder
#define BASIC_BLOCK BasicBlock::BasicBlockBuilder
#define GRAPH Graph

int main()
{
    // auto ins1 = INST(1, Opcode::ADD, 1, 2);
    // auto ins2 = INST(2, Opcode::JMP, 3);
    // auto ins3 = INST(3, Opcode::PHI, 1, 2, 3, 4);
    // ins1->Dump();
    // ins2->Dump();
    // ins3->Dump();
    LectureTest();
    return 0;
}
