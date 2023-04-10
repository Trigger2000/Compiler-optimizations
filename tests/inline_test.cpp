#include "gtest/gtest.h"

#include "pass/inlining.h"
#include "ir/ir_builder.h"

#define INST irb.InstBuilder
#define BASIC_BLOCK irb.BasicBlockBuilder
#define GRAPH irb.GraphBuilder

struct ExpectedInst {
    Opcode expected_opcode_;
    std::vector<uint32_t> expected_inputs_;
    std::vector<uint32_t> expected_users_;
};

struct ExpectedBB {
    std::vector<uint32_t> expected_preds_;
    std::vector<uint32_t> expected_succs_;
    std::vector<ExpectedInst> expected_insts_;
};

void CheckBasicBlock(BasicBlock* bb, ExpectedBB expected_bb)
{
    int index = 0;
    for (auto pred: bb->GetPreds()) {
        ASSERT_EQ(pred->GetId(), expected_bb.expected_preds_[index]);
        index++;
    }

    index = 0;
    for (auto succ: bb->GetSuccs()) {
        ASSERT_EQ(succ->GetId(), expected_bb.expected_succs_[index]);
        index++;
    }

    index = 0;
    for (auto inst = bb->GetFirstInst(); inst != nullptr; inst = inst->GetNext(), index++) {
        ASSERT_EQ(inst->GetOpcode(), expected_bb.expected_insts_[index].expected_opcode_);
        int user_index = 0;
        for (auto user: inst->GetUsers()) {
            ASSERT_EQ(user->GetId(), expected_bb.expected_insts_[index].expected_users_[user_index]);
            user_index++;
        }
        switch (inst->GetType())
        {
        case Type::InstWithOneInput:
        {
            ASSERT_EQ(inst->CastToInstWithOneInput()->GetInput1()->GetId(), expected_bb.expected_insts_[index].expected_inputs_[0]);
            break;
        }
        case Type::InstWithTwoInputs:
        {
            ASSERT_EQ(inst->CastToInstWithTwoInputs()->GetInput1()->GetId(), expected_bb.expected_insts_[index].expected_inputs_[0]);
            ASSERT_EQ(inst->CastToInstWithTwoInputs()->GetInput2()->GetId(), expected_bb.expected_insts_[index].expected_inputs_[1]);
            break;
        }
        case Type::InstConstant:
        {
            ASSERT_EQ(inst->CastToInstConstant()->GetConstant(), expected_bb.expected_insts_[index].expected_inputs_[0]);
            break;
        }
        case Type::InstJmp:
        {
            ASSERT_EQ(inst->CastToInstJmp()->GetTargetBB()->GetId(), expected_bb.expected_insts_[index].expected_inputs_[0]);
            break;
        }
        case Type::InstPhi:
        {
            int phi_input_index = 0;
            auto phi_insts = inst->CastToInstPhi()->GetInputInst();
            auto phi_bbs = inst->CastToInstPhi()->GetInputBB();
            for (int i = 0; i < phi_insts.size(); ++i) {
                ASSERT_EQ(phi_insts[i]->GetId(), expected_bb.expected_insts_[index].expected_inputs_[phi_input_index]);
                ASSERT_EQ(phi_bbs[i]->GetId(), expected_bb.expected_insts_[index].expected_inputs_[phi_input_index + 1]);
                phi_input_index += 2;
            }
        }
        default:
            break;
        }
    }
}

TEST(INLINING_TEST, TEST1) {
    IrBuilder irb;
    /*
            |---1---|
            |       |
            v       v
            2       3
            |       |
            |-->4<--|
                |
                v
               ret
    */
    Graph* callee = GRAPH({
        BASIC_BLOCK<1, 2, 3>({
            INST<Opcode::PARAMETER>(1),
            INST<Opcode::PARAMETER>(2),
            INST<Opcode::PARAMETER>(3),
            INST<Opcode::CONSTANT>(4, 42),
            INST<Opcode::CMP>(5, 3, 4),
            INST<Opcode::JA>(6, 3)
        }),
        BASIC_BLOCK<2, 4>({
            INST<Opcode::ADD>(7, 1, 2),
        }),
        BASIC_BLOCK<3, 4>({
            INST<Opcode::SUB>(8, 1, 2),
        }),
        BASIC_BLOCK<4>({
            INST<Opcode::PHI>(9, 7, 2, 8, 3),
            INST<Opcode::RET>(10, 9),
        })
    });

    irb = IrBuilder();
    Graph* caller = GRAPH({
        BASIC_BLOCK<5>({
            INST<Opcode::CONSTANT>(11, 314),
            INST<Opcode::CONSTANT>(12, 271),
            INST<Opcode::CONSTANT>(13, 50),
            INST<Opcode::CALL_STATIC>(14, callee, 11, 12, 13),
            INST<Opcode::RET>(15, 14),
        })
    });
    caller->RunPass<Inlining>();
    ASSERT_EQ(caller->GetBasicBlocks().size(), 6);

    CheckBasicBlock(caller->GetBasicBlocks()[0], {{}, {1}, {
        {Opcode::CONSTANT, {42}, {5}},
        {Opcode::CONSTANT, {314}, {7, 8}},
        {Opcode::CONSTANT, {271}, {7, 8}},
        {Opcode::CONSTANT, {50}, {5}},
        }
    });

    CheckBasicBlock(caller->GetBasicBlocks()[1], {{5}, {2, 3}, {
        {Opcode::CMP, {13, 4}, {}},
        {Opcode::JA, {3}, {}},
        }
    });

    CheckBasicBlock(caller->GetBasicBlocks()[2], {{1}, {4}, {
        {Opcode::ADD, {11, 12}, {9}},
        }
    });

    CheckBasicBlock(caller->GetBasicBlocks()[3], {{1}, {4}, {
        {Opcode::SUB, {11, 12}, {9}},
        }
    });

    CheckBasicBlock(caller->GetBasicBlocks()[4], {{2, 3}, {caller->GetBasicBlocks()[5]->GetId()}, {
        {Opcode::PHI, {7, 2, 8, 3}, {15}},
        }
    });

    CheckBasicBlock(caller->GetBasicBlocks()[5], {{4}, {}, {
        {Opcode::RET, {9}, {}},
        }
    });
}

TEST(INLINING_TEST, TEST2) {
    IrBuilder irb;
    /*
            1<---|
            |    |
            v    |
            2----|
            |
            v
         ret void
    */
    Graph* callee = GRAPH({
        BASIC_BLOCK<1, 2>({
            INST<Opcode::PARAMETER>(1),
            INST<Opcode::CONSTANT>(2, 1),
            INST<Opcode::CONSTANT>(3, 777),
            INST<Opcode::ADD>(4, 1, 2),
        }),
        BASIC_BLOCK<2, 1>({
            INST<Opcode::CMP>(5, 4, 3),
            INST<Opcode::JA>(6, 1),
            INST<Opcode::RET_VOID>(7),
        })
    });

    irb = IrBuilder();
    Graph* caller = GRAPH({
        BASIC_BLOCK<5>({
            INST<Opcode::CONSTANT>(8, 555),
            INST<Opcode::CALL_STATIC>(9, callee, 8),
            INST<Opcode::RET_VOID>(10),
        })
    });
    caller->RunPass<Inlining>();
    ASSERT_EQ(caller->GetBasicBlocks().size(), 4);

    CheckBasicBlock(caller->GetBasicBlocks()[0], {{}, {1}, {
        {Opcode::CONSTANT, {777}, {5}},
        {Opcode::CONSTANT, {1}, {4}},
        {Opcode::CONSTANT, {555}, {4}},
        }
    });

    CheckBasicBlock(caller->GetBasicBlocks()[1], {{2, 5}, {2}, {
        {Opcode::ADD, {8, 2}, {5}},
        }
    });

    CheckBasicBlock(caller->GetBasicBlocks()[2], {{1}, {1, caller->GetBasicBlocks()[3]->GetId()}, {
        {Opcode::CMP, {4, 3}, {}},
        {Opcode::JA, {1}, {}},
        }
    });

    CheckBasicBlock(caller->GetBasicBlocks()[3], {{2}, {}, {
        {Opcode::RET_VOID, {}, {}},
        }
    });
}

TEST(INLINING_TEST, TEST3) {
    IrBuilder irb;
    /*
            |---1---|
            |       |
            v       v
            2       3
            |       |
            v       v
           ret     ret
    */
    Graph* callee = GRAPH({
        BASIC_BLOCK<1, 2, 3>({
            INST<Opcode::PARAMETER>(1),
            INST<Opcode::PARAMETER>(2),
            INST<Opcode::PARAMETER>(3),
            INST<Opcode::CONSTANT>(4, 42),
            INST<Opcode::CMP>(5, 3, 4),
            INST<Opcode::JA>(6, 3)
        }),
        BASIC_BLOCK<2>({
            INST<Opcode::ADD>(7, 1, 2),
            INST<Opcode::RET>(8, 7),
        }),
        BASIC_BLOCK<3>({
            INST<Opcode::SUB>(9, 1, 2),
            INST<Opcode::RET>(10, 9),
        }),
    });

    irb = IrBuilder();
    Graph* caller = GRAPH({
        BASIC_BLOCK<5>({
            INST<Opcode::CONSTANT>(11, 314),
            INST<Opcode::CONSTANT>(12, 271),
            INST<Opcode::CONSTANT>(13, 50),
            INST<Opcode::CALL_STATIC>(14, callee, 11, 12, 13),
            INST<Opcode::RET>(15, 14),
        })
    });
    caller->RunPass<Inlining>();
    ASSERT_EQ(caller->GetBasicBlocks().size(), 5);

    CheckBasicBlock(caller->GetBasicBlocks()[0], {{}, {1}, {
        {Opcode::CONSTANT, {42}, {5}},
        {Opcode::CONSTANT, {314}, {7, 9}},
        {Opcode::CONSTANT, {271}, {7, 9}},
        {Opcode::CONSTANT, {50}, {5}},
        }
    });

    CheckBasicBlock(caller->GetBasicBlocks()[1], {{5}, {2, 3}, {
        {Opcode::CMP, {13, 4}, {}},
        {Opcode::JA, {3}, {}},
        }
    });

    CheckBasicBlock(caller->GetBasicBlocks()[2], {{1}, {caller->GetBasicBlocks()[4]->GetId()}, {
        {Opcode::ADD, {11, 12}, {caller->GetBasicBlocks()[4]->GetFirstInst()->GetId()}},
        }
    });

    CheckBasicBlock(caller->GetBasicBlocks()[3], {{1}, {caller->GetBasicBlocks()[4]->GetId()}, {
        {Opcode::SUB, {11, 12}, {caller->GetBasicBlocks()[4]->GetFirstInst()->GetId()}},
        }
    });

    CheckBasicBlock(caller->GetBasicBlocks()[4], {{2, 3}, {}, {
        {Opcode::PHI, {7, 2, 9, 3}, {15}},
        {Opcode::RET, {caller->GetBasicBlocks()[4]->GetFirstInst()->GetId()}, {}},
        }
    });
}

TEST(INLINING_TEST, TEST4) {
    IrBuilder irb;
    /*
            |---1---|
            |       |
            v       v
            2       3
            |       |
            v       v
           ret    throw
    */
    Graph* callee = GRAPH({
        BASIC_BLOCK<1, 2, 3>({
            INST<Opcode::PARAMETER>(1),
            INST<Opcode::PARAMETER>(2),
            INST<Opcode::PARAMETER>(3),
            INST<Opcode::CONSTANT>(4, 42),
            INST<Opcode::CMP>(5, 3, 4),
            INST<Opcode::JA>(6, 3)
        }),
        BASIC_BLOCK<2>({
            INST<Opcode::ADD>(7, 1, 2),
            INST<Opcode::RET>(8, 7),
        }),
        BASIC_BLOCK<3>({
            INST<Opcode::SUB>(9, 1, 2),
            INST<Opcode::THROW>(10),
        }),
    });

    irb = IrBuilder();
    Graph* caller = GRAPH({
        BASIC_BLOCK<5>({
            INST<Opcode::CONSTANT>(11, 314),
            INST<Opcode::CONSTANT>(12, 271),
            INST<Opcode::CONSTANT>(13, 50),
            INST<Opcode::CALL_STATIC>(14, callee, 11, 12, 13),
            INST<Opcode::RET>(15, 14),
        })
    });
    caller->RunPass<Inlining>();
    ASSERT_EQ(caller->GetBasicBlocks().size(), 5);
    
    CheckBasicBlock(caller->GetBasicBlocks()[0], {{}, {1}, {
        {Opcode::CONSTANT, {42}, {5}},
        {Opcode::CONSTANT, {314}, {7, 9}},
        {Opcode::CONSTANT, {271}, {7, 9}},
        {Opcode::CONSTANT, {50}, {5}},
        }
    });

    CheckBasicBlock(caller->GetBasicBlocks()[1], {{5}, {2, 3}, {
        {Opcode::CMP, {13, 4}, {}},
        {Opcode::JA, {3}, {}},
        }
    });

    CheckBasicBlock(caller->GetBasicBlocks()[2], {{1}, {caller->GetBasicBlocks()[4]->GetId()}, {
        {Opcode::ADD, {11, 12}, {caller->GetBasicBlocks()[4]->GetFirstInst()->GetId()}},
        }
    });

    CheckBasicBlock(caller->GetBasicBlocks()[3], {{1}, {caller->GetBasicBlocks()[4]->GetId()}, {
        {Opcode::SUB, {11, 12}, {caller->GetBasicBlocks()[4]->GetFirstInst()->GetId()}},
        {Opcode::THROW, {}, {}},
        }
    });

    CheckBasicBlock(caller->GetBasicBlocks()[4], {{2, 3}, {}, {
        {Opcode::RET, {7}, {}},
        }
    });
}
