#include "gtest/gtest.h"

#include "pass/const_folding.h"

#define INST Inst::InstBuilder
#define BASIC_BLOCK BasicBlock::BasicBlockBuilder
#define GRAPH Graph

// res = 8 - 2
TEST(CONST_FOLDING_TEST, TEST1) {
    Graph g = GRAPH{
        BASIC_BLOCK<1>({
            INST(1, Opcode::CONSTANT, 2),
            INST(2, Opcode::CONSTANT, 8),
            INST(3, Opcode::SUB, 2, 1),
        }),
    };
    g.RunPass<ConstFolding>();
    auto bb = g.GetBasicBlocks()[0];
    ASSERT_EQ(bb->GetSize(), 1);
    ASSERT_EQ(bb->GetFirstInst()->GetOpcode(), Opcode::CONSTANT);
    ASSERT_EQ(bb->GetFirstInst()->GetConstant(), 6);
}

// res = 8 >> 2
TEST(CONST_FOLDING_TEST, TEST2) {
    Graph g = GRAPH{
        BASIC_BLOCK<1>({
            INST(1, Opcode::CONSTANT, 2),
            INST(2, Opcode::CONSTANT, 8),
            INST(3, Opcode::SHR, 2, 1),
        }),
    };
    g.RunPass<ConstFolding>();
    auto bb = g.GetBasicBlocks()[0];
    ASSERT_EQ(bb->GetSize(), 1);
    ASSERT_EQ(bb->GetFirstInst()->GetOpcode(), Opcode::CONSTANT);
    ASSERT_EQ(bb->GetFirstInst()->GetConstant(), 2);
}

// res = 8 ^ 2
TEST(CONST_FOLDING_TEST, TEST3) {
    Graph g = GRAPH{
        BASIC_BLOCK<1>({
            INST(1, Opcode::CONSTANT, 2),
            INST(2, Opcode::CONSTANT, 8),
            INST(3, Opcode::XOR, 2, 1),
        }),
    };
    g.RunPass<ConstFolding>();
    auto bb = g.GetBasicBlocks()[0];
    ASSERT_EQ(bb->GetSize(), 1);
    ASSERT_EQ(bb->GetFirstInst()->GetOpcode(), Opcode::CONSTANT);
    ASSERT_EQ(bb->GetFirstInst()->GetConstant(), 10);
}

// res = (100 - 64 >> 3) ^ 20
TEST(CONST_FOLDING_TEST, TEST4) {
    Graph g = GRAPH{
        BASIC_BLOCK<1>({
            INST(1, Opcode::CONSTANT, 100),
            INST(2, Opcode::CONSTANT, 64),
            INST(3, Opcode::CONSTANT, 3),
            INST(4, Opcode::CONSTANT, 20),
            INST(5, Opcode::SHR, 2, 3),
            INST(6, Opcode::SUB, 1, 5),
            INST(7, Opcode::XOR, 6, 4),
        }),
    };

    g.RunPass<ConstFolding>();

    auto bb = g.GetBasicBlocks()[0];
    ASSERT_EQ(bb->GetSize(), 1);
    ASSERT_EQ(bb->GetFirstInst()->GetOpcode(), Opcode::CONSTANT);
    ASSERT_EQ(bb->GetFirstInst()->GetConstant(), 72);
}

// write more tests...
// TEST(CONST_FOLDING_TEST, TEST5) {
//     Graph g = GRAPH{
//         BASIC_BLOCK<1>({
//             INST(1, Opcode::PARAMETER),
//             INST(2, Opcode::CONSTANT, 100),
//             INST(3, Opcode::CONSTANT, 64),
//             INST(4, Opcode::CONSTANT, 3),
//             INST(5, Opcode::CONSTANT, 20),
//             INST(6, Opcode::SHR, 2, 3),
//             INST(7, Opcode::SUB, 1, 5),
//             INST(8, Opcode::XOR, 6, 4),
//         }),
//     };

//     for (int i = 0; i < 5; ++i) {
//         g.RunPass<ConstFolding>();
//     }

//     auto bb = g.GetBasicBlocks()[0];
//     ASSERT_EQ(bb->GetSize(), 1);
//     ASSERT_EQ(bb->GetFirstInst()->GetOpcode(), Opcode::CONSTANT);
//     ASSERT_EQ(bb->GetFirstInst()->GetConstant(), 72);
// }
