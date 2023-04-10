#include "gtest/gtest.h"

#include "ir/ir_builder.h"
#include "pass/const_folding.h"

#define INST irb.InstBuilder
#define BASIC_BLOCK irb.BasicBlockBuilder
#define GRAPH irb.GraphBuilder

// res = 8 - 2
TEST(CONST_FOLDING_TEST, TEST1) {
    IrBuilder irb;
    Graph* g = GRAPH({
        BASIC_BLOCK<1>({
            INST<Opcode::CONSTANT>(1, 2),
            INST<Opcode::CONSTANT>(2, 8),
            INST<Opcode::SUB>(3, 2, 1),
        }),
    });
    g->RunPass<ConstFolding>();
    auto bb = g->GetBasicBlocks()[0];
    ASSERT_EQ(bb->GetSize(), 1);
    ASSERT_EQ(bb->GetFirstInst()->GetOpcode(), Opcode::CONSTANT);
    ASSERT_EQ(bb->GetFirstInst()->CastToInstConstant()->GetConstant(), 6);
}

// res = 8 >> 2
TEST(CONST_FOLDING_TEST, TEST2) {
    IrBuilder irb;
    Graph *g = GRAPH({
        BASIC_BLOCK<1>({
            INST<Opcode::CONSTANT>(1, 2),
            INST<Opcode::CONSTANT>(2, 8),
            INST<Opcode::SHR>(3, 2, 1),
        }),
    });
    g->RunPass<ConstFolding>();
    auto bb = g->GetBasicBlocks()[0];
    ASSERT_EQ(bb->GetSize(), 1);
    ASSERT_EQ(bb->GetFirstInst()->GetOpcode(), Opcode::CONSTANT);
    ASSERT_EQ(bb->GetFirstInst()->CastToInstConstant()->GetConstant(), 2);
}

// res = 8 ^ 2
TEST(CONST_FOLDING_TEST, TEST3) {
    IrBuilder irb;
    Graph *g = GRAPH({
        BASIC_BLOCK<1>({
            INST<Opcode::CONSTANT>(1, 2),
            INST<Opcode::CONSTANT>(2, 8),
            INST<Opcode::XOR>(3, 2, 1),
        }),
    });
    g->RunPass<ConstFolding>();
    auto bb = g->GetBasicBlocks()[0];
    ASSERT_EQ(bb->GetSize(), 1);
    ASSERT_EQ(bb->GetFirstInst()->GetOpcode(), Opcode::CONSTANT);
    ASSERT_EQ(bb->GetFirstInst()->CastToInstConstant()->GetConstant(), 10);
}

// res = (100 - 64 >> 3) ^ 20
TEST(CONST_FOLDING_TEST, TEST4) {
    IrBuilder irb;
    Graph *g = GRAPH({
        BASIC_BLOCK<1>({
            INST<Opcode::CONSTANT>(1, 100),
            INST<Opcode::CONSTANT>(2, 64),
            INST<Opcode::CONSTANT>(3, 3),
            INST<Opcode::CONSTANT>(4, 20),
            INST<Opcode::SHR>(5, 2, 3),
            INST<Opcode::SUB>(6, 1, 5),
            INST<Opcode::XOR>(7, 6, 4),
        }),
    });

    g->RunPass<ConstFolding>();

    auto bb = g->GetBasicBlocks()[0];
    ASSERT_EQ(bb->GetSize(), 1);
    ASSERT_EQ(bb->GetFirstInst()->GetOpcode(), Opcode::CONSTANT);
    ASSERT_EQ(bb->GetFirstInst()->CastToInstConstant()->GetConstant(), 72);
}
