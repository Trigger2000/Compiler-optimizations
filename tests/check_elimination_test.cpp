#include "gtest/gtest.h"

#include "ir/ir_builder.h"
#include "pass/check_elimination.h"

#define INST irb.InstBuilder
#define BASIC_BLOCK irb.BasicBlockBuilder
#define GRAPH irb.GraphBuilder

void CheckUsers(Inst* inst, std::vector<uint32_t> expected)
{
    uint32_t index = 0;
    ASSERT_EQ(inst->GetUsers().size(), expected.size());
    for (auto item: inst->GetUsers()) {
        ASSERT_EQ(item->GetId(), expected[index]);
        ++index;
    }
}

TEST(CHECK_ELIMINATION_TEST, TEST1) {
    IrBuilder irb;
    Graph* g = GRAPH({
        BASIC_BLOCK<0>({
            INST<Opcode::CONSTANT>(1, 42),
            INST<Opcode::CONSTANT>(2, 27),
            INST<Opcode::SUB>(3, 2, 1),
            INST<Opcode::CHECK_EQ_ZERO>(4, 3),
            INST<Opcode::SUB>(5, 2, 1),
            INST<Opcode::CHECK_EQ_ZERO>(6, 3),
            INST<Opcode::CHECK_EQ_ZERO>(7, 3),
            INST<Opcode::CHECK_EQ_ZERO>(8, 3),
            INST<Opcode::ADD>(9, 2, 1),
            INST<Opcode::CHECK_EQ_ZERO>(10, 3),
        }),
    });
    g->RunPass<CheckElimination>();
    ASSERT_NE(g->GetInstById(4), nullptr);
    ASSERT_EQ(g->GetInstById(6), nullptr);
    ASSERT_EQ(g->GetInstById(6), nullptr);
    ASSERT_EQ(g->GetInstById(7), nullptr);
    ASSERT_EQ(g->GetInstById(8), nullptr);
    ASSERT_EQ(g->GetInstById(10), nullptr);
    CheckUsers(g->GetInstById(3), {4});
}

TEST(CHECK_ELIMINATION_TEST, TEST2) {
    IrBuilder irb;
    Graph* g = GRAPH({
        BASIC_BLOCK<0, 1>({
            INST<Opcode::CONSTANT>(1, 42),
            INST<Opcode::CONSTANT>(2, 27),
            INST<Opcode::SUB>(3, 2, 1),
            INST<Opcode::CHECK_EQ_ZERO>(4, 3),
            INST<Opcode::SUB>(5, 2, 1),
            INST<Opcode::CHECK_EQ_ZERO>(6, 3),
            INST<Opcode::CHECK_EQ_ZERO>(7, 3),
            INST<Opcode::ADD>(9, 2, 1),
        }),
        BASIC_BLOCK<1>({
            INST<Opcode::ADD>(10, 9, 5),
            INST<Opcode::CHECK_EQ_ZERO>(11, 3),
            INST<Opcode::CHECK_EQ_ZERO>(12, 3),
        }),
    });
    g->RunPass<CheckElimination>();
    ASSERT_NE(g->GetInstById(4), nullptr);
    ASSERT_EQ(g->GetInstById(6), nullptr);
    ASSERT_EQ(g->GetInstById(7), nullptr);
    ASSERT_EQ(g->GetInstById(11), nullptr);
    ASSERT_EQ(g->GetInstById(12), nullptr);
    CheckUsers(g->GetInstById(3), {4});
}

TEST(CHECK_ELIMINATION_TEST, TEST3) {
    IrBuilder irb;
    Graph* g = GRAPH({
        BASIC_BLOCK<0>({
            INST<Opcode::CONSTANT>(1, 42),
            INST<Opcode::CONSTANT>(2, 27),
            INST<Opcode::SUB>(3, 2, 1),
            INST<Opcode::CHECK_EQ>(4, 3, 1),
            INST<Opcode::SUB>(5, 2, 1),
            INST<Opcode::CHECK_EQ>(6, 3, 1),
            INST<Opcode::CHECK_EQ>(7, 1, 3),
            INST<Opcode::CHECK_EQ>(8, 3, 2),
            INST<Opcode::ADD>(9, 2, 1),
            INST<Opcode::CHECK_EQ>(10, 9, 1),
        }),
    });
    g->RunPass<CheckElimination>();
    ASSERT_NE(g->GetInstById(4), nullptr);
    ASSERT_EQ(g->GetInstById(6), nullptr);
    ASSERT_EQ(g->GetInstById(7), nullptr);
    ASSERT_NE(g->GetInstById(8), nullptr);
    ASSERT_NE(g->GetInstById(10), nullptr);
    CheckUsers(g->GetInstById(1), {3, 4, 5, 10, 9});
    CheckUsers(g->GetInstById(2), {3, 5, 8, 9});
    CheckUsers(g->GetInstById(3), {4, 8});
    CheckUsers(g->GetInstById(9), {10});
}

TEST(CHECK_ELIMINATION_TEST, TEST4) {
    IrBuilder irb;
    Graph* g = GRAPH({
        BASIC_BLOCK<0, 1>({
            INST<Opcode::CONSTANT>(1, 42),
            INST<Opcode::CONSTANT>(2, 27),
            INST<Opcode::SUB>(3, 2, 1),
            INST<Opcode::CHECK_EQ>(4, 3, 1),
            INST<Opcode::SUB>(5, 2, 1),
            INST<Opcode::CHECK_EQ>(6, 1, 3),
            INST<Opcode::CHECK_EQ>(7, 3, 5),
            INST<Opcode::ADD>(9, 2, 1),
        }),
        BASIC_BLOCK<1>({
            INST<Opcode::ADD>(10, 9, 5),
            INST<Opcode::CHECK_EQ>(11, 1, 3),
            INST<Opcode::CHECK_EQ>(12, 3, 1),
        }),
    });
    g->RunPass<CheckElimination>();
    ASSERT_NE(g->GetInstById(4), nullptr);
    ASSERT_EQ(g->GetInstById(6), nullptr);
    ASSERT_NE(g->GetInstById(7), nullptr);
    ASSERT_EQ(g->GetInstById(11), nullptr);
    ASSERT_EQ(g->GetInstById(12), nullptr);
    CheckUsers(g->GetInstById(1), {3, 4, 5, 9});
    CheckUsers(g->GetInstById(3), {4, 7});
    CheckUsers(g->GetInstById(5), {7, 10});
}

TEST(CHECK_ELIMINATION_TEST, TEST5) {
    IrBuilder irb;
    Graph* g = GRAPH({
        BASIC_BLOCK<0, 1>({
            INST<Opcode::CONSTANT>(1, 42),
            INST<Opcode::CONSTANT>(2, 27),
            INST<Opcode::SUB>(3, 2, 1),
            INST<Opcode::CHECK_EQ>(4, 3, 1),
            INST<Opcode::CHECK_EQ_ZERO>(5, 3),
            INST<Opcode::SUB>(6, 2, 1),
            INST<Opcode::CHECK_EQ>(7, 1, 3),
            INST<Opcode::CHECK_EQ_ZERO>(8, 3),
            INST<Opcode::ADD>(9, 2, 1),
        }),
        BASIC_BLOCK<1>({
            INST<Opcode::ADD>(10, 9, 5),
            INST<Opcode::CHECK_EQ>(11, 1, 3),
            INST<Opcode::CHECK_EQ_ZERO>(12, 3),
        }),
    });
    g->RunPass<CheckElimination>();
    ASSERT_NE(g->GetInstById(4), nullptr);
    ASSERT_NE(g->GetInstById(5), nullptr);
    ASSERT_EQ(g->GetInstById(7), nullptr);
    ASSERT_EQ(g->GetInstById(8), nullptr);
    ASSERT_EQ(g->GetInstById(11), nullptr);
    ASSERT_EQ(g->GetInstById(12), nullptr);
    CheckUsers(g->GetInstById(1), {3, 4, 6, 9});
    CheckUsers(g->GetInstById(3), {4, 5});
}