#include "gtest/gtest.h"

#include "pass/liveness_analysis.h"
#include "ir/ir_builder.h"

#define INST irb.InstBuilder
#define BASIC_BLOCK irb.BasicBlockBuilder
#define GRAPH irb.GraphBuilder

void DumpLiveIntervals(Graph* g)
{
    for (auto item: g->GetLiveIntervals()) {
        std::cout << item.first->GetId() << ": ["
                  << item.second.GetStart() << ", "
                  << item.second.GetEnd() << ")\n";
    }
}

void CheckLiveIntervals(Graph* g, std::unordered_map<uint32_t, LiveRange> expected)
{
    ASSERT_EQ(g->GetLiveIntervals().size(), expected.size());
    for (auto interval: g->GetLiveIntervals()) {
        uint32_t cur_id = interval.first->GetId();
        ASSERT_EQ(expected.count(cur_id), 1);
        ASSERT_EQ(interval.second.GetStart(), expected[cur_id].GetStart());
        ASSERT_EQ(interval.second.GetEnd(), expected[cur_id].GetEnd());
    }
}

TEST(LIVENESS_TEST, TEST1) {
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
    g->RunPass<LivenessAnalysis>();
    CheckLiveIntervals(g, {
        {0, {2, 22}}, {1, {4, 8}}, {2, {6, 24}},
        {3, {8, 24}}, {4, {8, 18}}, {5, {0, 0}},
        {6, {0, 0}}, {7, {16, 22}}, {8, {18, 22}},
        {9, {0, 0}}, {10, {24, 26}}, {11, {0, 0}},
    });
}

TEST(LIVENESS_TEST, TEST2)
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
    // no difference with previous test,
    // since JMP_EQ becomes JMP_NE
    g->RunPass<LivenessAnalysis>();
    ASSERT_EQ(g->GetInstById(6)->GetOpcode(), Opcode::JMP_NE);
    CheckLiveIntervals(g, {
        {0, {2, 22}}, {1, {4, 8}}, {2, {6, 24}},
        {3, {8, 24}}, {4, {8, 18}}, {5, {0, 0}},
        {6, {0, 0}}, {7, {16, 22}}, {8, {18, 22}},
        {9, {0, 0}}, {10, {24, 26}}, {11, {0, 0}},
    });
}

TEST(LIVENESS_TEST, TEST3)
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
    g->RunPass<LivenessAnalysis>();
    CheckLiveIntervals(g, {
        {0, {2, 28}}, {1, {4, 28}}, {2, {6, 16}},
        {5, {0, 0}}, {6, {0, 0}}, {7, {26, 28}},
        {8, {28, 30}}, {9, {0, 0}}, {10, {16, 18}},
        {11, {0, 0}}, {12, {0, 0}},
    });
}

TEST(LIVENESS_TEST, TEST4)
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
    g->RunPass<LivenessAnalysis>();
    CheckLiveIntervals(g, {
        {0, {2, 24}}, {1, {4, 28}}, {2, {6, 16}},
        {5, {0, 0}}, {6, {0, 0}}, {7, {22, 24}},
        {8, {24, 26}}, {9, {16, 20}}, {10, {0, 0}},
        {11, {26, 28}}, {12, {28, 30}}, {13, {0, 0}},
    });
}

TEST(LIVENESS_TEST, TEST5)
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
    g->RunPass<LivenessAnalysis>();
    CheckLiveIntervals(g, {
        {0, {2, 24}}, {1, {4, 28}}, {2, {6, 24}},
        {5, {0, 0}}, {6, {0, 0}}, {7, {16, 18}},
        {8, {18, 22}}, {9, {24, 26}}, {10, {0, 0}},
        {11, {26, 28}}, {12, {28, 30}}, {13, {0, 0}},
    });
}
