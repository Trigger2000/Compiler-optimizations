#include "graph.h"

#define INST InstNode::InstBuilder
#define BASIC_BLOCK BasicBlock::BasicBlockBuilder
#define GRAPH Graph

int main()
{
    Graph g = GRAPH{
        BASIC_BLOCK<1, 2, 3>({
            INST<1>(Opcode::ADD, 1, 2, 3),
            INST<2>(Opcode::ADD, 4, 5, 6),
            INST<3>(Opcode::ADD, 7, 8, 9),
            INST<4>(Opcode::ADD, 10, 11, 12)
        }),
        BASIC_BLOCK<2, 3>({
            INST<1>(Opcode::SUB, 1, 2, 3),
            INST<2>(Opcode::SUB, 4, 5, 6),
            INST<3>(Opcode::SUB, 7, 8, 9),
            INST<4>(Opcode::SUB, 10, 11, 12)
        }),
        BASIC_BLOCK<3>({
            INST<1>(Opcode::MULI, 1, 2, 3),
            INST<2>(Opcode::MULI, 4, 5, 6),
            INST<3>(Opcode::MULI, 7, 8, 9),
            INST<4>(Opcode::MULI, 10, 11, 12)
        })
    };

    g.Dump();
    Graph::GraphDestroyer(&g);
    return 0;
}
