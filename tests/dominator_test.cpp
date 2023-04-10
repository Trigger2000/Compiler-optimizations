#include "gtest/gtest.h"

#include "pass/dom_tree_slow.h"
#include "pass/dom_tree_fast.h"
#include "ir/ir_builder.h"

#define INST irb.InstBuilder
#define BASIC_BLOCK irb.BasicBlockBuilder
#define GRAPH irb.GraphBuilder

void check_dominators(BasicBlock* bb, std::vector<uint32_t> expect_doms)
{
    auto comparator = [](BasicBlock* rhs, BasicBlock* lhs) { return rhs->GetId() < lhs->GetId(); };
    auto doms = bb->GetDominators();
    std::sort(doms.begin(), doms.end(), comparator);
    ASSERT_EQ(doms.size(), expect_doms.size());
    for (int i = 0; i < doms.size(); ++i) {
        ASSERT_EQ(doms[i]->GetId(), expect_doms[i]);
    }
}

// test case from https://llvm.org/devmtg/2017-10/slides/Kuderski-Dominator_Trees.pdf
TEST(DOMINATOR_TEST, DOMINATOR_TEST_LLVM) {
    IrBuilder irb;
    Graph *g = GRAPH({
        BASIC_BLOCK<0, 1>({}),
        BASIC_BLOCK<1, 2, 5>({}),
        BASIC_BLOCK<2, 3, 4>({}),
        BASIC_BLOCK<3, 6>({}),
        BASIC_BLOCK<4, 6>({}),
        BASIC_BLOCK<5, 7>({}),
        BASIC_BLOCK<6, 7>({}),
        BASIC_BLOCK<7, 5, 8>({}),
        BASIC_BLOCK<8>({})
    });

    g->RunPass<DomTreeSlow>();
    auto bbs = g->GetBasicBlocks();

    check_dominators(bbs[0], {0});
    check_dominators(bbs[1], {0, 1});
    check_dominators(bbs[2], {0, 1, 2});
    check_dominators(bbs[3], {0, 1, 2, 3});
    check_dominators(bbs[4], {0, 1, 2, 4});
    check_dominators(bbs[5], {0, 1, 5});
    check_dominators(bbs[6], {0, 1, 2, 6});
    check_dominators(bbs[7], {0, 1, 7});
    check_dominators(bbs[8], {0, 1, 7, 8});
}

// test case from https://www.cs.princeton.edu/courses/archive/fall03/cs528/handouts/a%20fast%20algorithm%20for%20finding.pdf
TEST(DOMINATOR_TEST, DOMINATOR_TEST_ARTICLE) {
    IrBuilder irb;
    Graph *g = GRAPH({
        BASIC_BLOCK<1, 2, 3, 7>({}),
        BASIC_BLOCK<2, 4, 5>({}),
        BASIC_BLOCK<3, 6, 7, 11>({}),
        BASIC_BLOCK<4, 8>({}),
        BASIC_BLOCK<5, 8, 9>({}),
        BASIC_BLOCK<6, 10>({}),
        BASIC_BLOCK<7, 11>({}),
        BASIC_BLOCK<8, 12>({}),
        BASIC_BLOCK<9, 8>({}),
        BASIC_BLOCK<10, 6, 12>({}),
        BASIC_BLOCK<11, 13>({}),
        BASIC_BLOCK<12, 8, 1>({}),
        BASIC_BLOCK<13, 10>({})
    });

    g->RunPass<DomTreeFast>();
}

// test case 1 from lecture
TEST(DOMINATOR_TEST, DOMINATOR_TEST_SLOW_1) {
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

    g->RunPass<DomTreeSlow>();
    auto bbs = g->GetBasicBlocks();

    check_dominators(bbs[0], {0});
    check_dominators(bbs[1], {0, 1});
    check_dominators(bbs[2], {0, 1, 2});
    check_dominators(bbs[3], {0, 1, 3});
    check_dominators(bbs[4], {0, 1, 4, 5});
    check_dominators(bbs[5], {0, 1, 5});
    check_dominators(bbs[6], {0, 1, 5, 6});
}

// test case 2 from lecture
TEST(DOMINATOR_TEST, DOMINATOR_TEST_SLOW_2) {
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

    g->RunPass<DomTreeSlow>();
    auto bbs = g->GetBasicBlocks();

    check_dominators(bbs[0], {0});
    check_dominators(bbs[1], {0, 1});
    check_dominators(bbs[2], {0, 1, 2});
    check_dominators(bbs[3], {0, 1, 2, 3});
    check_dominators(bbs[4], {0, 1, 2, 3, 4});
    check_dominators(bbs[5], {0, 1, 2, 3, 4, 5});
    check_dominators(bbs[6], {0, 1, 2, 3, 4, 5, 6});
    check_dominators(bbs[7], {0, 1, 2, 3, 4, 5, 6, 7});
    check_dominators(bbs[8], {0, 1, 2, 3, 4, 5, 6, 8});
    check_dominators(bbs[9], {0, 1, 9});
    check_dominators(bbs[10], {0, 1, 2, 3, 4, 5, 6, 8, 10});
}

// test case 3 from lecture
TEST(DOMINATOR_TEST, DOMINATOR_TEST_SLOW_3) {
    IrBuilder irb;
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

    g->RunPass<DomTreeSlow>();
    auto bbs = g->GetBasicBlocks();

    check_dominators(bbs[0], {0});
    check_dominators(bbs[1], {0, 1});
    check_dominators(bbs[2], {0, 1, 2});
    check_dominators(bbs[3], {0, 1, 3});
    check_dominators(bbs[4], {0, 1, 4});
    check_dominators(bbs[5], {0, 1, 4, 5});
    check_dominators(bbs[6], {0, 1, 6});
    check_dominators(bbs[7], {0, 1, 4, 5, 7});
    check_dominators(bbs[8], {0, 1, 8});
}

// test case 1 from lecture
TEST(DOMINATOR_TEST, DOMINATOR_TEST_FAST_1) {
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

    g->RunPass<DomTreeFast>();
    auto bbs = g->GetBasicBlocks();

    ASSERT_EQ(bbs[0]->GetIDom(), nullptr);
    ASSERT_EQ(bbs[1]->GetIDom(), bbs[0]);
    ASSERT_EQ(bbs[2]->GetIDom(), bbs[1]);
    ASSERT_EQ(bbs[3]->GetIDom(), bbs[1]);
    ASSERT_EQ(bbs[4]->GetIDom(), bbs[5]);
    ASSERT_EQ(bbs[5]->GetIDom(), bbs[1]);
    ASSERT_EQ(bbs[6]->GetIDom(), bbs[5]);
}

// test case 2 from lecture
TEST(DOMINATOR_TEST, DOMINATOR_TEST_FAST_2) {
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

    g->RunPass<DomTreeFast>();
    auto bbs = g->GetBasicBlocks();

    ASSERT_EQ(bbs[0]->GetIDom(), nullptr);
    ASSERT_EQ(bbs[1]->GetIDom(), bbs[0]);
    ASSERT_EQ(bbs[2]->GetIDom(), bbs[1]);
    ASSERT_EQ(bbs[3]->GetIDom(), bbs[2]);
    ASSERT_EQ(bbs[4]->GetIDom(), bbs[3]);
    ASSERT_EQ(bbs[5]->GetIDom(), bbs[4]);
    ASSERT_EQ(bbs[6]->GetIDom(), bbs[5]);
    ASSERT_EQ(bbs[7]->GetIDom(), bbs[6]);
    ASSERT_EQ(bbs[8]->GetIDom(), bbs[6]);
    ASSERT_EQ(bbs[9]->GetIDom(), bbs[1]);
    ASSERT_EQ(bbs[10]->GetIDom(), bbs[8]);
}

// test case 3 from lecture
TEST(DOMINATOR_TEST, DOMINATOR_TEST_FAST_3) {
    IrBuilder irb;
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

    g->RunPass<DomTreeFast>();
    auto bbs = g->GetBasicBlocks();

    ASSERT_EQ(bbs[0]->GetIDom(), nullptr);
    ASSERT_EQ(bbs[1]->GetIDom(), bbs[0]);
    ASSERT_EQ(bbs[2]->GetIDom(), bbs[1]);
    ASSERT_EQ(bbs[3]->GetIDom(), bbs[1]);
    ASSERT_EQ(bbs[4]->GetIDom(), bbs[1]);
    ASSERT_EQ(bbs[5]->GetIDom(), bbs[4]);
    ASSERT_EQ(bbs[6]->GetIDom(), bbs[1]);
    ASSERT_EQ(bbs[7]->GetIDom(), bbs[5]);
    ASSERT_EQ(bbs[8]->GetIDom(), bbs[1]);
}
