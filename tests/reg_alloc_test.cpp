#include "gtest/gtest.h"

#include "pass/reg_alloc.h"
#include "ir/ir_builder.h"

#define INST irb.InstBuilder
#define BASIC_BLOCK irb.BasicBlockBuilder
#define GRAPH irb.GraphBuilder

constexpr size_t TEST_REG_NUM = 3;

void DumpAllocatedIntervals(Graph* g)
{
    for (auto item: g->GetLiveIntervals()) {
        std::cout << item.first->GetId() << ": ["
                  << item.second->GetStart() << ", "
                  << item.second->GetEnd() << ") -> ";
        if (item.second->GetIsStackLocation()) {
            std::cout << "S" << item.second->GetLocation();
        } else {
            std::cout << "R" << item.second->GetLocation();
        }
        std::cout << "\n";
    }
}

void CheckAllocatedIntervals(Graph* g, std::unordered_map<uint32_t, std::string> expected)
{
    ASSERT_EQ(g->GetLiveIntervals().size(), expected.size());
    for (auto interval: g->GetLiveIntervals()) {
        uint32_t inst_id = interval.first->GetId();
        uint32_t location = std::stoi(expected[inst_id].substr(1, expected[inst_id].size() - 1));
        bool is_stack = false;
        if (expected[inst_id][0] == 'S') {
            is_stack = true;
        }
        ASSERT_EQ(interval.second->GetLocation(), location);
        ASSERT_EQ(interval.second->GetIsStackLocation(), is_stack);
    }
}

TEST(REG_ALLOC_TEST, TEST1) {
    IrBuilder irb;
    /*
                0
                |
                v
          |---->1----|
          |False|    |True
          |     v    v
          |-----2    3
    */
    Graph *g = GRAPH({
        BASIC_BLOCK<0, 1>({
            INST<Opcode::CONSTANT>(0, 1),
            INST<Opcode::CONSTANT>(1, 10),
            INST<Opcode::CONSTANT>(2, 20),
        }),
        BASIC_BLOCK<1, 3, 2>({
            INST<Opcode::PHI>(3, 7, 2, 0, 0),
            INST<Opcode::PHI>(4, 8, 2, 1, 0),
            INST<Opcode::CMP>(5, 4, 0),
            INST<Opcode::JMP_EQ>(6, 3),
        }),
        BASIC_BLOCK<2, 1>({
            INST<Opcode::MUL>(7, 3, 4),
            INST<Opcode::SUB>(8, 4, 0),
            INST<Opcode::JMP>(9, 1),
        }),
        BASIC_BLOCK<3>({
            INST<Opcode::ADD>(10, 2, 3),
            INST<Opcode::RET_VOID>(11),
        }),
    });
    RegAlloc::SetRegCount(TEST_REG_NUM);
    g->RunPass<RegAlloc>();
    CheckAllocatedIntervals(g, {
        {0, "R0"}, {1, "R1"}, {2, "S1"},
        {3, "S0"}, {4, "R1"}, {7, "R2"},
        {8, "R1"}, {10, "R0"},
    });
}

TEST(REG_ALLOC_TEST, TEST2)
{
    IrBuilder irb;
    /*
                0
                |
                v
           |--->1----|
           |True|    |False
           |    v    v
           |----2    3
    */
    Graph *g = GRAPH({
        BASIC_BLOCK<0, 1>({
            INST<Opcode::CONSTANT>(0, 1),
            INST<Opcode::CONSTANT>(1, 10),
            INST<Opcode::CONSTANT>(2, 20),
        }),
        // order of successors differs from the previous test
        BASIC_BLOCK<1, 2, 3>({
            INST<Opcode::PHI>(3, 7, 2, 0, 0),
            INST<Opcode::PHI>(4, 8, 2, 1, 0),
            INST<Opcode::CMP>(5, 4, 0),
            INST<Opcode::JMP_EQ>(6, 3),
        }),
        BASIC_BLOCK<2, 1>({
            INST<Opcode::MUL>(7, 3, 4),
            INST<Opcode::SUB>(8, 4, 0),
            INST<Opcode::JMP>(9, 1),
        }),
        BASIC_BLOCK<3>({
            INST<Opcode::ADD>(10, 2, 3),
            INST<Opcode::RET_VOID>(11),
        }),
    });
    RegAlloc::SetRegCount(TEST_REG_NUM);
    g->RunPass<RegAlloc>();
    CheckAllocatedIntervals(g, {
        {0, "R0"}, {1, "R1"}, {2, "S1"},
        {3, "S0"}, {4, "R1"}, {7, "R2"},
        {8, "R1"}, {10, "R0"},
    });
}

TEST(REG_ALLOC_TEST, TEST3)
{
    IrBuilder irb;
    /*
                0
                |
                v
                1----|
            True|    |False
                v    v
                2    3
                     |
                     v
                     4
    */
    Graph *g = GRAPH({
        BASIC_BLOCK<0, 1>({
            INST<Opcode::CONSTANT>(0, 1),
            INST<Opcode::CONSTANT>(1, 10),
            INST<Opcode::CONSTANT>(2, 20),
        }),
        BASIC_BLOCK<1, 2, 3>({
            INST<Opcode::CMP>(5, 0, 1),
            INST<Opcode::JMP_EQ>(6, 2),
        }),
        BASIC_BLOCK<2>({
            INST<Opcode::MUL>(7, 0, 1),
            INST<Opcode::SUB>(8, 1, 0),
            INST<Opcode::RET_VOID>(9),
        }),
        BASIC_BLOCK<3, 4>({
            INST<Opcode::ADD>(10, 2, 0),
            INST<Opcode::JMP>(11, 4),
        }),
        BASIC_BLOCK<4>({
            INST<Opcode::RET_VOID>(12),
        }),
    });
    RegAlloc::SetRegCount(TEST_REG_NUM);
    g->RunPass<RegAlloc>();
    CheckAllocatedIntervals(g, {
        {0, "R0"}, {1, "R1"}, {2, "R2"},
        {7, "R2"}, {8, "R0"}, {10, "R2"},
    });
}

TEST(REG_ALLOC_TEST, TEST4)
{
    IrBuilder irb;
    /*
                0
                |
                v
                1----|
            True|    |False
                v    v
                2    3
                |    |
                v    |
                4<---|
                |
                v
                5
    */
    Graph *g = GRAPH({
        BASIC_BLOCK<0, 1>({
            INST<Opcode::CONSTANT>(0, 1),
            INST<Opcode::CONSTANT>(1, 10),
            INST<Opcode::CONSTANT>(2, 20),
        }),
        BASIC_BLOCK<1, 2, 3>({
            INST<Opcode::CMP>(5, 1, 0),
            INST<Opcode::JMP_EQ>(6, 2),
        }),
        BASIC_BLOCK<2, 4>({
            INST<Opcode::MUL>(7, 0, 1),
            INST<Opcode::SUB>(8, 7, 0),
        }),
        BASIC_BLOCK<3, 4>({
            INST<Opcode::ADD>(9, 2, 0),
            INST<Opcode::JMP>(10, 4),
        }),
        BASIC_BLOCK<4, 5>({
            INST<Opcode::PHI>(11, 9, 3, 8, 2),
            INST<Opcode::ADD>(12, 11, 1),
        }),
        BASIC_BLOCK<5>({
            INST<Opcode::RET_VOID>(13)
        }),
    });
    RegAlloc::SetRegCount(TEST_REG_NUM);
    g->RunPass<RegAlloc>();
    CheckAllocatedIntervals(g, {
        {0, "R0"}, {1, "R1"}, {2, "R2"},
        {7, "R2"}, {8, "R0"}, {9, "R2"},
        {11, "R0"}, {12, "R0"}
    });
}

TEST(REG_ALLOC_TEST, TEST5)
{
    IrBuilder irb;
    /*
                0
                |
                v
                1----|
           False|    |True
                v    v
                2    3
                |    |
                v    |
                4<---|
                |
                v
                5
    */
    Graph *g = GRAPH({
        BASIC_BLOCK<0, 1>({
            INST<Opcode::CONSTANT>(0, 1),
            INST<Opcode::CONSTANT>(1, 10),
            INST<Opcode::CONSTANT>(2, 20),
        }),
        // order of successors differs from the previous test
        BASIC_BLOCK<1, 3, 2>({
            INST<Opcode::CMP>(5, 1, 0),
            INST<Opcode::JMP_EQ>(6, 2),
        }),
        BASIC_BLOCK<2, 4>({
            INST<Opcode::MUL>(7, 0, 1),
            INST<Opcode::SUB>(8, 7, 0),
            INST<Opcode::JMP>(10, 4),
        }),
        BASIC_BLOCK<3, 4>({
            INST<Opcode::ADD>(9, 2, 0),
        }),
        BASIC_BLOCK<4, 5>({
            INST<Opcode::PHI>(11, 9, 3, 8, 2),
            INST<Opcode::ADD>(12, 11, 1),
        }),
        BASIC_BLOCK<5>({
            INST<Opcode::RET_VOID>(13)
        }),
    });
    RegAlloc::SetRegCount(TEST_REG_NUM);
    g->RunPass<RegAlloc>();
    CheckAllocatedIntervals(g, {
        {0, "R0"}, {1, "S0"}, {2, "R2"},
        {7, "R1"}, {8, "R1"}, {9, "R0"},
        {11, "R0"}, {12, "R0"}
    });
}
