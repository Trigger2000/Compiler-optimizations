#include "gtest/gtest.h"

#include "pass/peephole.h"
#include "ir/ir_builder.h"

#define INST irb.InstBuilder
#define BASIC_BLOCK irb.BasicBlockBuilder
#define GRAPH irb.GraphBuilder

void CheckUsers(BasicBlock* bb, std::vector<std::vector<int32_t>> users) {
    int inst_counter = 0;
    for (Inst* inst = bb->GetFirstInst(); inst != nullptr; inst = inst->GetNext(), ++inst_counter) {
        int user_counter = 0;
        for (auto inst_user: inst->GetUsers()) {
            ASSERT_EQ(users[inst_counter][user_counter], inst_user->GetId());
            ++user_counter;
        }
    }
}

// checks inputs in instructions that have two inputs
void CheckInstsWithTwoInputs(BasicBlock* bb, std::vector<std::vector<int32_t>> inputs) {
    int inst_counter = 0;
    for (Inst* inst = bb->GetFirstInst(); inst != nullptr; inst = inst->GetNext()) {
        if (inst->GetType() != Type::InstWithTwoInputs)
            continue;
        ASSERT_EQ(inst->CastToInstWithTwoInputs()->GetInput1()->GetId(), inputs[inst_counter][0]);
        ASSERT_EQ(inst->CastToInstWithTwoInputs()->GetInput2()->GetId(), inputs[inst_counter][1]);
        ++inst_counter;
    }
}

TEST(PEEPHOLE_TEST, TEST_SUB) {
    // case 1
    IrBuilder irb;
    Graph* g1 = GRAPH({
        BASIC_BLOCK<1>({
            INST<Opcode::PARAMETER>(1),
            INST<Opcode::CONSTANT>(2, 0),
            INST<Opcode::CONSTANT>(3, 32),
            INST<Opcode::SUB>(4, 1, 2),
            INST<Opcode::SUB>(5, 1, 3),
            INST<Opcode::ADD>(6, 4, 5),
        }),
    });
    g1->RunPass<Peephole>();
    // g1->Dump();
    auto bb = g1->GetBasicBlocks()[0];
    ASSERT_EQ(bb->GetSize(), 4);
    ASSERT_EQ(g1->GetInstById(2), nullptr);
    ASSERT_EQ(g1->GetInstById(4), nullptr);
    CheckUsers(bb, {{6, 5}, {5}, {6}, {-1}});
    CheckInstsWithTwoInputs(bb, {{1, 3}, {1, 5}});

    // TODO fix me
    // case 2 + case 1
    // irb = IrBuilder();
    // Graph* g2 = GRAPH({
    //     BASIC_BLOCK<2>({
    //         INST<Opcode::PARAMETER>(1),
    //         INST<Opcode::CONSTANT>(2, 32),
    //         INST<Opcode::SUB>(3, 1, 1),
    //         INST<Opcode::SUB>(4, 1, 3),
    //         INST<Opcode::ADD>(5, 1, 2),
    //     }),
    // });
    // g2->RunPass<Peephole>();
    // g2->Dump();
    // bb = g2->GetBasicBlocks()[0];
    // ASSERT_EQ(bb->GetSize(), 3);
    // ASSERT_EQ(g2->GetInstById(3), nullptr);
    // ASSERT_EQ(g2->GetInstById(4), nullptr);
    // CheckUsers(bb, {{5}, {5}, {-1}});
    // CheckInstsWithTwoInputs(bb, {{1, 2}});

    // case 3
    irb = IrBuilder();
    Graph* g3 = GRAPH({
        BASIC_BLOCK<1>({
            INST<Opcode::PARAMETER>(1),
            INST<Opcode::PARAMETER>(2),
            INST<Opcode::ADD>(3, 1, 2),
            INST<Opcode::SUB>(4, 3, 2),
            INST<Opcode::ADD>(5, 3, 4),
        }),
    });
    g3->RunPass<Peephole>();
    // g3.Dump();
    bb = g3->GetBasicBlocks()[0];
    ASSERT_EQ(bb->GetSize(), 4);
    ASSERT_EQ(g3->GetInstById(4), nullptr);
    CheckUsers(bb, {{3, 5}, {3}, {5}, {-1}});
    CheckInstsWithTwoInputs(bb, {{1, 2}, {3, 1}});

    // case 4
    irb = IrBuilder();
    Graph* g4 = GRAPH({
        BASIC_BLOCK<1>({
            INST<Opcode::PARAMETER>(1),
            INST<Opcode::PARAMETER>(2),
            INST<Opcode::SUB>(3, 1, 2),
            INST<Opcode::SUB>(4, 1, 3),
            INST<Opcode::ADD>(5, 4, 1),
        }),
    });
    g4->RunPass<Peephole>();
    // g4.Dump();
    bb = g4->GetBasicBlocks()[0];
    ASSERT_EQ(bb->GetSize(), 3);
    ASSERT_EQ(g4->GetInstById(3), nullptr);
    ASSERT_EQ(g4->GetInstById(4), nullptr);
    CheckUsers(bb, {{5}, {5}, {-1}});
    CheckInstsWithTwoInputs(bb, {{2, 1}});

    // case 5
    irb = IrBuilder();
    Graph* g5 = GRAPH({
        BASIC_BLOCK<1>({
            INST<Opcode::PARAMETER>(1),
            INST<Opcode::CONSTANT>(2, 8),
            INST<Opcode::CONSTANT>(3, 16),
            INST<Opcode::SUB>(4, 1, 2),
            INST<Opcode::SUB>(5, 4, 3),
            INST<Opcode::ADD>(6, 5, 4),
        }),
    });
    g5->RunPass<Peephole>();
    // g5.Dump();
    bb = g5->GetBasicBlocks()[0];
    int32_t new_inst_id = static_cast<int32_t>(bb->GetFirstInst()->GetId());
    ASSERT_EQ(bb->GetFirstInst()->CastToInstConstant()->GetConstant(), 24);
    ASSERT_EQ(bb->GetSize(), 6);
    ASSERT_EQ(g5->GetInstById(3), nullptr);
    ASSERT_NE(g5->GetInstById(new_inst_id), nullptr);
    CheckUsers(bb, {{5}, {4}, {4}, {6}, {6}, {-1}});
    CheckInstsWithTwoInputs(bb, {{1, 2}, {1, new_inst_id}, {5, 4}});
}

TEST(PEEPHOLE_TEST, TEST_SHR) {
    // case 1
    IrBuilder irb;
    Graph* g1 = GRAPH({
        BASIC_BLOCK<1>({
            INST<Opcode::PARAMETER>(1),
            INST<Opcode::CONSTANT>(2, 0),
            INST<Opcode::CONSTANT>(3, 4),
            INST<Opcode::SHR>(4, 1, 2),
            INST<Opcode::SHR>(5, 1, 3),
            INST<Opcode::ADD>(6, 4, 5),
        }),
    });
    g1->RunPass<Peephole>();
    // g1.Dump();
    auto bb = g1->GetBasicBlocks()[0];
    ASSERT_EQ(bb->GetSize(), 4);
    ASSERT_EQ(g1->GetInstById(2), nullptr);
    ASSERT_EQ(g1->GetInstById(4), nullptr);
    CheckUsers(bb, {{6, 5}, {5}, {6}, {-1}});
    CheckInstsWithTwoInputs(bb, {{1, 3}, {1, 5}});

    // case 2
    irb = IrBuilder();
    Graph* g2 = GRAPH({
        BASIC_BLOCK<1>({
            INST<Opcode::PARAMETER>(1),
            INST<Opcode::PARAMETER>(2),
            INST<Opcode::SHL>(3, 1, 2),
            INST<Opcode::SHR>(4, 3, 2),
            INST<Opcode::ADD>(5, 3, 4),
        }),
    });
    g2->RunPass<Peephole>();
    // g2.Dump();
    bb = g2->GetBasicBlocks()[0];
    ASSERT_EQ(bb->GetSize(), 4);
    ASSERT_EQ(g2->GetInstById(4), nullptr);
    CheckUsers(bb, {{3, 5}, {3}, {5}, {-1}});
    CheckInstsWithTwoInputs(bb, {{1, 2}, {3, 1}});

    // case 3
    irb = IrBuilder();
    Graph* g3 = GRAPH({
        BASIC_BLOCK<1>({
            INST<Opcode::PARAMETER>(1),
            INST<Opcode::CONSTANT>(2, 8),
            INST<Opcode::CONSTANT>(3, 16),
            INST<Opcode::SHR>(4, 1, 2),
            INST<Opcode::SHR>(5, 4, 3),
            INST<Opcode::ADD>(6, 5, 4),
        }),
    });
    g3->RunPass<Peephole>();
    // g3.Dump();
    bb = g3->GetBasicBlocks()[0];
    int32_t new_inst_id = static_cast<int32_t>(bb->GetFirstInst()->GetId());
    ASSERT_EQ(bb->GetFirstInst()->CastToInstConstant()->GetConstant(), 24);
    ASSERT_EQ(bb->GetSize(), 6);
    ASSERT_EQ(g3->GetInstById(3), nullptr);
    ASSERT_NE(g3->GetInstById(new_inst_id), nullptr);
    CheckUsers(bb, {{5}, {4}, {4}, {6}, {6}, {-1}});
    CheckInstsWithTwoInputs(bb, {{1, 2}, {1, new_inst_id}, {5, 4}});
}

TEST(PEEPHOLE_TEST, TEST_XOR) {
    // case 1
    IrBuilder irb;
    Graph* g1 = GRAPH({
        BASIC_BLOCK<1>({
            INST<Opcode::PARAMETER>(1),
            INST<Opcode::CONSTANT>(2, 0),
            INST<Opcode::CONSTANT>(3, 4),
            INST<Opcode::XOR>(4, 1, 2),
            INST<Opcode::XOR>(5, 1, 3),
            INST<Opcode::ADD>(6, 4, 5),
        }),
    });
    g1->RunPass<Peephole>();
    // g1.Dump();
    auto bb = g1->GetBasicBlocks()[0];
    ASSERT_EQ(bb->GetSize(), 4);
    ASSERT_EQ(g1->GetInstById(2), nullptr);
    ASSERT_EQ(g1->GetInstById(4), nullptr);
    CheckUsers(bb, {{6, 5}, {5}, {6}, {-1}});
    CheckInstsWithTwoInputs(bb, {{1, 3}, {1, 5}});

    // case 2 + case 1
    irb = IrBuilder();
    Graph* g2 = GRAPH({
        BASIC_BLOCK<1>({
            INST<Opcode::PARAMETER>(1),
            INST<Opcode::CONSTANT>(2, 32),
            INST<Opcode::SUB>(3, 1, 1),
            INST<Opcode::SUB>(4, 1, 3),
            INST<Opcode::ADD>(5, 1, 2),
        }),
    });
    g2->RunPass<Peephole>();
    // g2.Dump();
    bb = g2->GetBasicBlocks()[0];
    ASSERT_EQ(bb->GetSize(), 3);
    ASSERT_EQ(g2->GetInstById(3), nullptr);
    ASSERT_EQ(g2->GetInstById(4), nullptr);
    CheckUsers(bb, {{5}, {5}, {-1}});
    CheckInstsWithTwoInputs(bb, {{1, 2}});

    // case 3
    irb = IrBuilder();
    Graph* g3 = GRAPH({
        BASIC_BLOCK<1>({
            INST<Opcode::PARAMETER>(1),
            INST<Opcode::CONSTANT>(2, -1),
            INST<Opcode::XOR>(3, 1, 2),
            INST<Opcode::ADD>(4, 1, 3),
        }),
    });
    g3->RunPass<Peephole>();
    // g3.Dump();
    bb = g3->GetBasicBlocks()[0];
    int32_t new_inst_id = static_cast<int32_t>(g3->GetInstById(4)->GetPrev()->GetId());
    ASSERT_EQ(bb->GetSize(), 3);
    ASSERT_EQ(g3->GetInstById(2), nullptr);
    ASSERT_EQ(g3->GetInstById(3), nullptr);
    ASSERT_NE(g3->GetInstById(new_inst_id), nullptr);
    CheckUsers(bb, {{4, new_inst_id}, {4}, {-1}});
    CheckInstsWithTwoInputs(bb, {{1, new_inst_id}});
}
