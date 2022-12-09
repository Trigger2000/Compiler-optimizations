#include "gtest/gtest.h"

#include "pass/peephole.h"

#define INST Inst::InstBuilder
#define BASIC_BLOCK BasicBlock::BasicBlockBuilder
#define GRAPH Graph

TEST(PEEPHOLE_TEST, TEST_SUB) {
    // case 1
    Graph g1 = GRAPH{
        BASIC_BLOCK<1>({
            INST(1, Opcode::PARAMETER),
            INST(2, Opcode::CONSTANT, 0),
            INST(3, Opcode::CONSTANT, 32),
            INST(4, Opcode::SUB, 1, 2),
            INST(5, Opcode::SUB, 1, 3),
            INST(6, Opcode::ADD, 4, 5),
        }),
    };
    g1.RunPass<Peephole>();
    // g1.Dump();

    // case 2 + case 1
    Graph g2 = GRAPH{
        BASIC_BLOCK<1>({
            INST(1, Opcode::PARAMETER),
            INST(2, Opcode::CONSTANT, 32),
            INST(3, Opcode::SUB, 1, 1),
            INST(4, Opcode::SUB, 1, 3),
            INST(5, Opcode::ADD, 1, 2),
        }),
    };
    g2.RunPass<Peephole>();
    // g2.Dump();

    // case 3
    Graph g3 = GRAPH{
        BASIC_BLOCK<1>({
            INST(1, Opcode::PARAMETER),
            INST(2, Opcode::PARAMETER),
            INST(3, Opcode::ADD, 1, 2),
            INST(4, Opcode::SUB, 3, 2),
            INST(5, Opcode::ADD, 3, 4),
        }),
    };
    g3.RunPass<Peephole>();
    // g3.Dump();

    // case 4
    Graph g4 = GRAPH{
        BASIC_BLOCK<1>({
            INST(1, Opcode::PARAMETER),
            INST(2, Opcode::PARAMETER),
            INST(3, Opcode::SUB, 1, 2),
            INST(4, Opcode::SUB, 1, 3),
            INST(5, Opcode::ADD, 4, 1),
        }),
    };
    g4.RunPass<Peephole>();
    // g4.Dump();

    // case 5
    Graph g5 = GRAPH{
        BASIC_BLOCK<1>({
            INST(1, Opcode::PARAMETER),
            INST(2, Opcode::CONSTANT, 8),
            INST(3, Opcode::CONSTANT, 16),
            INST(4, Opcode::SUB, 1, 2),
            INST(5, Opcode::SUB, 4, 3),
            INST(6, Opcode::ADD, 5, 4),
        }),
    };
    g5.RunPass<Peephole>();
    // g5.Dump();
}

TEST(PEEPHOLE_TEST, TEST_SHR) {
    // case 1
    Graph g1 = GRAPH{
        BASIC_BLOCK<1>({
            INST(1, Opcode::PARAMETER),
            INST(2, Opcode::CONSTANT, 0),
            INST(3, Opcode::CONSTANT, 4),
            INST(4, Opcode::SHR, 1, 2),
            INST(5, Opcode::SHR, 1, 3),
            INST(6, Opcode::ADD, 4, 5),
        }),
    };
    g1.RunPass<Peephole>();
    // g1.Dump();

    // case 2
    Graph g2 = GRAPH{
        BASIC_BLOCK<1>({
            INST(1, Opcode::PARAMETER),
            INST(2, Opcode::PARAMETER),
            INST(3, Opcode::SHL, 1, 2),
            INST(4, Opcode::SHR, 3, 2),
            INST(5, Opcode::ADD, 3, 4),
        }),
    };
    g2.RunPass<Peephole>();
    // g2.Dump();

    // case 3
    Graph g3 = GRAPH{
        BASIC_BLOCK<1>({
            INST(1, Opcode::PARAMETER),
            INST(2, Opcode::CONSTANT, 8),
            INST(3, Opcode::CONSTANT, 16),
            INST(4, Opcode::SHR, 1, 2),
            INST(5, Opcode::SHR, 4, 3),
            INST(6, Opcode::ADD, 5, 4),
        }),
    };
    g3.RunPass<Peephole>();
    // g3.Dump();
}

TEST(PEEPHOLE_TEST, TEST_XOR) {
    // case 1
    Graph g1 = GRAPH{
        BASIC_BLOCK<1>({
            INST(1, Opcode::PARAMETER),
            INST(2, Opcode::CONSTANT, 0),
            INST(3, Opcode::CONSTANT, 4),
            INST(4, Opcode::XOR, 1, 2),
            INST(5, Opcode::XOR, 1, 3),
            INST(6, Opcode::ADD, 4, 5),
        }),
    };
    g1.RunPass<Peephole>();
    // g1.Dump();

    // case 2 + case 1
    Graph g2 = GRAPH{
        BASIC_BLOCK<1>({
            INST(1, Opcode::PARAMETER),
            INST(2, Opcode::CONSTANT, 32),
            INST(3, Opcode::SUB, 1, 1),
            INST(4, Opcode::SUB, 1, 3),
            INST(5, Opcode::ADD, 1, 2),
        }),
    };
    g2.RunPass<Peephole>();
    // g2.Dump();

    // case 3
    Graph g3 = GRAPH{
        BASIC_BLOCK<1>({
            INST(1, Opcode::PARAMETER),
            INST(2, Opcode::CONSTANT, -1),
            INST(3, Opcode::XOR, 1, 2),
            INST(4, Opcode::ADD, 1, 3),
        }),
    };
    g3.RunPass<Peephole>();
    // g3.Dump();
}
