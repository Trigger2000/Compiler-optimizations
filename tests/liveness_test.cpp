#include "gtest/gtest.h"

#include "ir/ir_builder.h"
#include "pass/liveness_analysis.h"

#define INST irb.InstBuilder
#define BASIC_BLOCK irb.BasicBlockBuilder
#define GRAPH irb.GraphBuilder

void CheckLinearOrder(std::vector<BasicBlock*> input, std::vector<uint32_t> expected)
{
    ASSERT_EQ(input.size(), expected.size());
    for (int i = 0; i < input.size(); ++i) {
        ASSERT_EQ(input[i]->GetId(), expected[i]);
    }
}

// test case from lecture
TEST(LIVENESS_TEST, LINEAR_ORDER_TEST)
{
    IrBuilder irb;
    /*                0
                      |
                      v
                  |---2---|
                  |       |
                  v       v
              |-->4------>3<---15-----|
              |   |       |           |
              |   v       v           |
              12  5       6<----|     |
              ^   |       |     |     |
              |   v       v     |     |
              |---11      7     10    |
                  |       |     |     |
                  v       v     |     |
                  13      8---->9     |
                  |       |           |
                  v       v           |
                  1       14----------|
    */
    Graph *g = GRAPH({
        BASIC_BLOCK<0, 2>({}),
        BASIC_BLOCK<1>({}),
        BASIC_BLOCK<2, 3, 4>({}),
        BASIC_BLOCK<3, 6>({}),
        BASIC_BLOCK<4, 3, 5>({}),
        BASIC_BLOCK<5, 11>({}),
        BASIC_BLOCK<6, 7>({}),
        BASIC_BLOCK<7, 8>({}),
        BASIC_BLOCK<8, 9, 14>({}),
        BASIC_BLOCK<9, 10>({}),
        BASIC_BLOCK<10, 6>({}),
        BASIC_BLOCK<11, 12, 13>({}),
        BASIC_BLOCK<12, 4>({}),
        BASIC_BLOCK<13, 1>({}),
        BASIC_BLOCK<14, 15>({}),
        BASIC_BLOCK<15, 3>({}),
    });

    g->RunPass<LivenessAnalysis>();
    CheckLinearOrder(g->GetLinearOrder(),
    {0, 2, 4, 5, 11, 12, 13, 1, 3, 6, 7, 8, 9, 10, 14, 15});
}

// test case from lecture
TEST(LIVENESS_TEST, LIVENESS_TEST_1)
{
    IrBuilder irb;
    /*
                0
                |
                v
           |--->1----|
           |    |    |
           |    v    v
           |----2    3
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
            INST<Opcode::PHI>(3, 7, 2, 0, 0),
            INST<Opcode::PHI>(4, 8, 2, 1, 0),
            INST<Opcode::CMP>(5, 4, 0),
            INST<Opcode::JA>(6, 3),
        }),
        BASIC_BLOCK<2, 1>({
            INST<Opcode::MUL>(7, 3, 4),
            INST<Opcode::SUB>(8, 4, 0),
            // there should be jump, but in the lecture it was omitted
        }),
        BASIC_BLOCK<3, 4>({
            INST<Opcode::ADD>(9, 2, 3),
            INST<Opcode::RET_VOID>(10),
        }),
        BASIC_BLOCK<4>({}),
    });
    g->RunPass<LivenessAnalysis>();
    CheckLinearOrder(g->GetLinearOrder(),
    {0, 1, 2, 3, 4});
}