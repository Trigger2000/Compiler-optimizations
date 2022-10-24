#include "gtest/gtest.h"

#include "ir/graph.h"

#define INST Inst::InstBuilder
#define BASIC_BLOCK BasicBlock::BasicBlockBuilder
#define GRAPH Graph

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
TEST(GRAPH_TEST, DOMINATOR_TEST_LLVM) {
    Graph g = GRAPH{
        BASIC_BLOCK<0, 1>({}),
        BASIC_BLOCK<1, 2, 5>({}),
        BASIC_BLOCK<2, 3, 4>({}),
        BASIC_BLOCK<3, 6>({}),
        BASIC_BLOCK<4, 6>({}),
        BASIC_BLOCK<5, 7>({}),
        BASIC_BLOCK<6, 7>({}),
        BASIC_BLOCK<7, 5, 8>({}),
        BASIC_BLOCK<8>({})
    };

    g.BuildDominatorTreeSlow();
    auto bbs = g.GetBasicBlocks();

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

// test case 1 from lecture
TEST(GRAPH_TEST, DOMINATOR_TEST_1) {
    Graph g = GRAPH{
        BASIC_BLOCK<0, 1>({}),
        BASIC_BLOCK<1, 2, 5>({}),
        BASIC_BLOCK<2, 3>({}),
        BASIC_BLOCK<3>({}),
        BASIC_BLOCK<4, 3>({}),
        BASIC_BLOCK<5, 4, 6>({}),
        BASIC_BLOCK<6, 3>({}),
    };

    g.BuildDominatorTreeSlow();
    auto bbs = g.GetBasicBlocks();

    check_dominators(bbs[0], {0});
    check_dominators(bbs[1], {0, 1});
    check_dominators(bbs[2], {0, 1, 2});
    check_dominators(bbs[3], {0, 1, 3});
    check_dominators(bbs[4], {0, 1, 4, 5});
    check_dominators(bbs[5], {0, 1, 5});
    check_dominators(bbs[6], {0, 1, 5, 6});
}

// test case 2 from lecture
TEST(GRAPH_TEST, DOMINATOR_TEST_2) {
    Graph g = GRAPH{
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
    };

    g.BuildDominatorTreeSlow();
    auto bbs = g.GetBasicBlocks();

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
TEST(GRAPH_TEST, DOMINATOR_TEST_3) {
    Graph g = GRAPH{
        BASIC_BLOCK<0, 1>({}),
        BASIC_BLOCK<1, 2, 4>({}),
        BASIC_BLOCK<2, 3>({}),
        BASIC_BLOCK<3, 6>({}),
        BASIC_BLOCK<4, 3, 5>({}),
        BASIC_BLOCK<5, 1, 7>({}),
        BASIC_BLOCK<6, 2, 8>({}),
        BASIC_BLOCK<7, 6, 8>({}),
        BASIC_BLOCK<8>({})
    };

    g.BuildDominatorTreeSlow();
    auto bbs = g.GetBasicBlocks();

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

TEST(GRAPH_TEST, TEST1) {
    /*
    bb1
        inputs      0
        movi        1, 1
        movi        2, 2
        u32tou64    3, 0
    bb2
        phi         4 (2, 1) (7, 3)
        cmp         4, 3
        ja          4
    bb3
        phi         5 (1, 1) (6, 3)
        mul         6, 5, 4
        addi        7, 4, 1
        jmp         2
    bb4
        phi         8 (1, 1) (6, 2)
        ret         8
    */

   // this is a meaningless graph!
//    Graph g = GRAPH{
//         BASIC_BLOCK<1, 2>({
//             INST(1, Opcode::PARAMETER),
//             // INST(2, Opcode::MOVI, 1, 1),
//             // INST(3, Opcode::MOVI, 2, 2),
//             INST(4, Opcode::CAST, 1)
//         }),
//         BASIC_BLOCK<2, 3, 4>({
//             INST(5, Opcode::PHI, 1, 1, 4, 1),
//             INST(6, Opcode::CMP, 4, 5),
//             INST(7, Opcode::JA, 12),
//         }),
//         BASIC_BLOCK<3, 2, 4>({
//             INST(8, Opcode::PHI, 1, 1, 6, 2),
//             INST(9, Opcode::MUL, 5, 4),
//             // INST(10, Opcode::ADDI, 7, 4, 1),
//             INST(11, Opcode::JMP, 5)
//         }),
//         BASIC_BLOCK<4>({
//             INST(12, Opcode::PHI, 1, 1, 6, 2),
//             INST(13, Opcode::RET, 8),
//         })
//     };

//     g.Dump();

    // BasicBlock *bb1 = g.GetBBbyId(1);
    // BasicBlock *bb2 = g.GetBBbyId(2);
    // BasicBlock *bb3 = g.GetBBbyId(3);
    // BasicBlock *bb4 = g.GetBBbyId(4);

    // const std::unordered_map<uint32_t, BasicBlock*>& preds1 = bb1->GetPreds();
    // const std::unordered_map<uint32_t, BasicBlock*>& preds2 = bb2->GetPreds();
    // const std::unordered_map<uint32_t, BasicBlock*>& preds3 = bb3->GetPreds();
    // const std::unordered_map<uint32_t, BasicBlock*>& preds4 = bb4->GetPreds();

    // assert(preds1.empty());
    // assert(preds2.find(1) != preds2.end());
    // assert(preds2.find(3) != preds2.end());
    // assert(preds3.find(2) != preds3.end());
    // assert(preds4.find(2) != preds4.end());
    // assert(preds4.find(3) != preds4.end());

    // const std::unordered_map<uint32_t, BasicBlock*>& succs1 = bb1->GetSuccs();
    // const std::unordered_map<uint32_t, BasicBlock*>& succs2 = bb2->GetSuccs();
    // const std::unordered_map<uint32_t, BasicBlock*>& succs3 = bb3->GetSuccs();
    // const std::unordered_map<uint32_t, BasicBlock*>& succs4 = bb4->GetSuccs();

    // assert(succs1.find(2) != succs1.end());
    // assert(succs2.find(3) != succs2.end());
    // assert(succs2.find(4) != succs2.end());
    // assert(succs3.find(2) != succs3.end());
    // assert(succs3.find(4) != succs3.end());
    // assert(succs4.empty());


    // Inst *bb1inst1 = bb1->GetFirstinst();
    // Inst *bb1inst2 = bb1->GetFirstinst()->GetNext();
    // Inst *bb1inst3 = bb1->GetLastInst()->GetPrev();
    // Inst *bb1inst4 = bb1->GetLastInst();

    // assert(bb1inst1->GetId() == 1);
    // assert(bb1inst2->GetId() == 2);
    // assert(bb1inst3->GetId() == 3);
    // assert(bb1inst4->GetId() == 4);

    // assert(bb1inst1->GetBBId() == 1);
    // assert(bb1inst2->GetBBId() == 1);
    // assert(bb1inst3->GetBBId() == 1);
    // assert(bb1inst4->GetBBId() == 1);

    // assert(bb1inst1->GetOpcode() == Opcode::PARAMETER);
    // assert(bb1inst2->GetOpcode() == Opcode::MOVI);
    // assert(bb1inst3->GetOpcode() == Opcode::MOVI);
    // assert(bb1inst4->GetOpcode() == Opcode::CAST);

    // assert(bb1inst1->GetType() == Type::InstParameter);
    // // assert(bb1inst2->GetType() == Type::InstWithTwoInputsImm);
    // // assert(bb1inst3->GetType() == Type::InstWithTwoInputsImm);
    // assert(bb1inst4->GetType() == Type::InstUtil);

    // assert(bb1inst1->GetInputs()[0] == 0);
    // // assert(bb1inst2->GetDstReg() == 1);
    // assert(bb1inst2->GetImm() == 1);
    // // assert(bb1inst3->GetDstReg() == 2);
    // assert(bb1inst3->GetImm() == 2);
    // assert(bb1inst4->GetSrcReg1() == 3);
    // assert(bb1inst4->GetSrcReg2() == 0);
}