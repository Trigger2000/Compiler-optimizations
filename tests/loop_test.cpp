#include "gtest/gtest.h"

#include "pass/loop_analyzer.h"
#include "ir/ir_builder.h"

#define INST irb.InstBuilder
#define BASIC_BLOCK irb.BasicBlockBuilder
#define GRAPH irb.GraphBuilder

void CheckLoopBlocks(Loop* loop, std::vector<uint32_t> expected_block_ids)
{
    auto loop_blocks = loop->GetBlocks();
    auto comparator = [](BasicBlock* rhs, BasicBlock* lhs) { return rhs->GetId() < lhs->GetId(); };
    std::sort(loop_blocks.begin(), loop_blocks.end(), comparator);
    ASSERT_EQ(loop->GetBlocks().size(), expected_block_ids.size());
    for (uint32_t i = 0; i < expected_block_ids.size(); ++i) {
        ASSERT_EQ(loop_blocks[i]->GetId(), expected_block_ids[i]);
    }
}

// test case 1 from lecture
TEST(LOOP_TEST, LOOP_TEST_1) {
    IrBuilder irb;
    /*
                0
                |
                v
            |---1---|
            |       |
            v       v
            2   4<--5
            |   |   |
            |   v   v
            |-->3<--6
    */
    Graph *g = GRAPH({
        BASIC_BLOCK<0, 1>({}),
        BASIC_BLOCK<1, 2, 5>({}),
        BASIC_BLOCK<2, 3>({}),
        BASIC_BLOCK<3>({}),
        BASIC_BLOCK<4, 3>({}),
        BASIC_BLOCK<5, 4, 6>({}),
        BASIC_BLOCK<6, 3>({}),
    });

    g->RunPass<LoopAnalyzer>();
    Loop* root_loop = g->GetRootLoop();
    ASSERT_EQ(root_loop->GetOuterLoop(), nullptr);
    ASSERT_EQ(root_loop->GetInnerLoops().size(), 0);
    CheckLoopBlocks(root_loop, {0, 1, 2, 3, 4, 5, 6});
}

// test case 2 from lecture
TEST(LOOP_TEST, LOOP_TEST_2) {
    IrBuilder irb;
    /*
                0
                |
                v
           |--->1----|
           |    |    |
           |    V    v
           | |->2<---9
           | |  |  
           | |  V  
           | ---3
           |    |
           |    v
           |    4<---|
           |    |    |
           |    v    |
           |    5----|
           |    |
           |    v
           7<---6--->8--->10
    */
    Graph *g = GRAPH({
        BASIC_BLOCK<0, 1>({}),
        BASIC_BLOCK<1, 2, 9>({}),
        BASIC_BLOCK<2, 3>({}),
        BASIC_BLOCK<3, 2, 4>({}),
        BASIC_BLOCK<4, 5>({}),
        BASIC_BLOCK<5, 4, 6>({}),
        BASIC_BLOCK<6, 7, 8>({}),
        BASIC_BLOCK<7, 1>({}),
        BASIC_BLOCK<8, 10>({}),
        BASIC_BLOCK<9, 2>({}),
        BASIC_BLOCK<10>({})
    });

    g->RunPass<LoopAnalyzer>();
    Loop* root_loop = g->GetRootLoop();
    ASSERT_EQ(root_loop->GetOuterLoop(), nullptr);
    ASSERT_EQ(root_loop->GetInnerLoops().size(), 1);
    CheckLoopBlocks(root_loop, {0, 8, 10});

    Loop* loop_7_1 = root_loop->GetInnerLoops()[0];
    ASSERT_EQ(loop_7_1->GetOuterLoop(), root_loop);
    ASSERT_EQ(loop_7_1->GetInnerLoops().size(), 2);
    CheckLoopBlocks(loop_7_1, {1, 2, 3, 4, 5, 6, 7, 9});

    Loop* loop_5_4 = loop_7_1->GetInnerLoops()[0];
    ASSERT_EQ(loop_5_4->GetOuterLoop(), loop_7_1);
    ASSERT_EQ(loop_5_4->GetInnerLoops().size(), 0);
    CheckLoopBlocks(loop_5_4, {4, 5});

    Loop* loop_3_2 = loop_7_1->GetInnerLoops()[1];
    ASSERT_EQ(loop_3_2->GetOuterLoop(), loop_7_1);
    ASSERT_EQ(loop_3_2->GetInnerLoops().size(), 0);
    CheckLoopBlocks(loop_3_2, {2, 3});
}

// test case 3 from lecture
TEST(LOOP_TEST, LOOP_TEST_3) {
    IrBuilder irb;
    /*
                0
                |
    |---------->|
    |           |
    |           v
    |       |---1
    |       |   |
    |       V   V
    |   |---4   2<--|
    |   |   |   |   | 
    |   V   |   V   |  
    |---5   |-->3   |
        |       |   |
        |       v   |
        |-->7-->6---|
            |   |
            |   v
            |-->8
    */
    Graph *g = GRAPH({
        BASIC_BLOCK<0, 1>({}),
        BASIC_BLOCK<1, 2, 4>({}),
        BASIC_BLOCK<2, 3>({}),
        BASIC_BLOCK<3, 6>({}),
        BASIC_BLOCK<4, 3, 5>({}),
        BASIC_BLOCK<5, 1, 7>({}),
        BASIC_BLOCK<6, 2, 8>({}),
        BASIC_BLOCK<7, 6, 8>({}),
        BASIC_BLOCK<8>({})
    });

    g->RunPass<LoopAnalyzer>();
    Loop* root_loop = g->GetRootLoop();
    ASSERT_EQ(root_loop->GetOuterLoop(), nullptr);
    ASSERT_EQ(root_loop->GetInnerLoops().size(), 1);
    CheckLoopBlocks(root_loop, {0, 2, 3, 6, 7, 8});

    Loop* loop_5_1 = root_loop->GetInnerLoops()[0];
    ASSERT_EQ(loop_5_1->GetOuterLoop(), root_loop);
    ASSERT_EQ(loop_5_1->GetInnerLoops().size(), 0);
    CheckLoopBlocks(loop_5_1, {1, 4, 5});
}

// test case 4 from lecture
TEST(LOOP_TEST, LOOP_TEST_4) {
    IrBuilder irb;
    /*
                0
                |
                v
            |---1<--|
            |   |   |
            v   v   |
            4   2   |
                |   |
                v   |
                3---|
    */
    Graph *g = GRAPH({
        BASIC_BLOCK<0, 1>({}),
        BASIC_BLOCK<1, 2, 4>({}),
        BASIC_BLOCK<2, 3>({}),
        BASIC_BLOCK<3, 1>({}),
        BASIC_BLOCK<4>({}),
    });

    g->RunPass<LoopAnalyzer>();
    Loop* root_loop = g->GetRootLoop();
    ASSERT_EQ(root_loop->GetOuterLoop(), nullptr);
    ASSERT_EQ(root_loop->GetInnerLoops().size(), 1);
    CheckLoopBlocks(root_loop, {0, 4});

    Loop* loop_3_1 = root_loop->GetInnerLoops()[0];
    ASSERT_EQ(loop_3_1->GetOuterLoop(), root_loop);
    ASSERT_EQ(loop_3_1->GetInnerLoops().size(), 0);
    CheckLoopBlocks(loop_3_1, {1, 2, 3});
}

// test case 5 from lecture
TEST(LOOP_TEST, LOOP_TEST_5) {
    IrBuilder irb;
    /*
                0
                |
                v
            |---1<--|
            |       |
            V       |
            2-->3-->4
            |   |
            |   v
            |-->5
    */
    Graph *g = GRAPH({
        BASIC_BLOCK<0, 1>({}),
        BASIC_BLOCK<1, 2>({}),
        BASIC_BLOCK<2, 3, 5>({}),
        BASIC_BLOCK<3, 4, 5>({}),
        BASIC_BLOCK<4, 1>({}),
        BASIC_BLOCK<5>({}),
    });

    g->RunPass<LoopAnalyzer>();
    Loop* root_loop = g->GetRootLoop();
    ASSERT_EQ(root_loop->GetOuterLoop(), nullptr);
    ASSERT_EQ(root_loop->GetInnerLoops().size(), 1);
    CheckLoopBlocks(root_loop, {0, 5});

    Loop* loop_4_1 = root_loop->GetInnerLoops()[0];
    ASSERT_EQ(loop_4_1->GetOuterLoop(), root_loop);
    ASSERT_EQ(loop_4_1->GetInnerLoops().size(), 0);
    CheckLoopBlocks(loop_4_1, {1, 2, 3, 4});
}

// test case 6 from lecture
TEST(LOOP_TEST, LOOP_TEST_6) {
    IrBuilder irb;
    /*
                0<----------|
                |           |
                v           |
            |---1<--|<--|   |
            |       |   |   |
            V       |   |   |
        |---2       3   |   |
        |   |       |   |   |
        v   |       |   |   |
        4   |-->5<--|   |   |
                |       |   |
                v       |   |
                6-------|   |
                |           |
                v           |
                7-----------|
    */
    Graph *g = GRAPH({
        BASIC_BLOCK<0, 1>({}),
        BASIC_BLOCK<1, 2, 3>({}),
        BASIC_BLOCK<2, 4, 5>({}),
        BASIC_BLOCK<3, 5>({}),
        BASIC_BLOCK<4>({}),
        BASIC_BLOCK<5, 6>({}),
        BASIC_BLOCK<6, 1, 7>({}),
        BASIC_BLOCK<7, 0>({}),
    });

    g->RunPass<LoopAnalyzer>();
    Loop* root_loop = g->GetRootLoop();
    ASSERT_EQ(root_loop->GetOuterLoop(), nullptr);
    ASSERT_EQ(root_loop->GetInnerLoops().size(), 1);
    CheckLoopBlocks(root_loop, {4});

    Loop* loop_7_0 = root_loop->GetInnerLoops()[0];
    ASSERT_EQ(loop_7_0->GetOuterLoop(), root_loop);
    ASSERT_EQ(loop_7_0->GetInnerLoops().size(), 1);
    CheckLoopBlocks(loop_7_0, {0, 1, 2, 3, 5, 6, 7});

    Loop* loop_6_1 = loop_7_0->GetInnerLoops()[0];
    ASSERT_EQ(loop_6_1->GetOuterLoop(), loop_7_0);
    ASSERT_EQ(loop_6_1->GetInnerLoops().size(), 0);
    CheckLoopBlocks(loop_6_1, {1, 2, 3, 5, 6});
}
