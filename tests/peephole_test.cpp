#include "gtest/gtest.h"

#include "pass/peephole.h"

#define INST Inst::InstBuilder
#define BASIC_BLOCK BasicBlock::BasicBlockBuilder
#define GRAPH Graph

void CheckUsers(BasicBlock* bb, std::vector<std::vector<int32_t>> users) {
    int inst_counter = 0;
    for (Inst* inst = bb->GetFirstInst(); inst != nullptr; inst = inst->GetNext(), ++inst_counter) {
        int user_counter = 0;
        for (auto inst_user: inst->GetUsers().GetUsers()) {
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
        ASSERT_EQ(inst->GetInput1()->GetInputId(), inputs[inst_counter][0]);
        ASSERT_EQ(inst->GetInput2()->GetInputId(), inputs[inst_counter][1]);
        ++inst_counter;
    }
}

TEST(PEEPHOLE_TEST, TEST_SUB) {
    // case 1
    Graph g1 = GRAPH{
        BASIC_BLOCK<1>({
            INST(1, Opcode::PARAMETER),
            INST(2, Opcode::CONSTANT, 0),
            INST(3, Opcode::CONSTANT, 32),
            INST(4, Opcode::SUB, 1, 2),
            INST(5, Opcode::SUB, 1, 3),
            INST(6, Opcode::ADD, 4, 5),
        }),
    };
    g1.RunPass<Peephole>();
    // g1.Dump();
    auto bb = g1.GetBasicBlocks()[0];
    ASSERT_EQ(bb->GetSize(), 4);
    ASSERT_EQ(g1.GetInstById(2), nullptr);
    ASSERT_EQ(g1.GetInstById(4), nullptr);
    CheckUsers(bb, {{5, 6}, {5}, {6}, {-1}});
    CheckInstsWithTwoInputs(bb, {{1, 3}, {1, 5}});

    // case 2 + case 1
    Graph g2 = GRAPH{
        BASIC_BLOCK<1>({
            INST(1, Opcode::PARAMETER),
            INST(2, Opcode::CONSTANT, 32),
            INST(3, Opcode::SUB, 1, 1),
            INST(4, Opcode::SUB, 1, 3),
            INST(5, Opcode::ADD, 1, 2),
        }),
    };
    g2.RunPass<Peephole>();
    // g2.Dump();
    bb = g2.GetBasicBlocks()[0];
    ASSERT_EQ(bb->GetSize(), 3);
    ASSERT_EQ(g2.GetInstById(3), nullptr);
    ASSERT_EQ(g2.GetInstById(4), nullptr);
    CheckUsers(bb, {{5}, {5}, {-1}});
    CheckInstsWithTwoInputs(bb, {{1, 2}});

    // case 3
    Graph g3 = GRAPH{
        BASIC_BLOCK<1>({
            INST(1, Opcode::PARAMETER),
            INST(2, Opcode::PARAMETER),
            INST(3, Opcode::ADD, 1, 2),
            INST(4, Opcode::SUB, 3, 2),
            INST(5, Opcode::ADD, 3, 4),
        }),
    };
    g3.RunPass<Peephole>();
    // g3.Dump();
    bb = g3.GetBasicBlocks()[0];
    ASSERT_EQ(bb->GetSize(), 4);
    ASSERT_EQ(g3.GetInstById(4), nullptr);
    CheckUsers(bb, {{3, 5}, {3}, {5}, {-1}});
    CheckInstsWithTwoInputs(bb, {{1, 2}, {3, 1}});

    // case 4
    Graph g4 = GRAPH{
        BASIC_BLOCK<1>({
            INST(1, Opcode::PARAMETER),
            INST(2, Opcode::PARAMETER),
            INST(3, Opcode::SUB, 1, 2),
            INST(4, Opcode::SUB, 1, 3),
            INST(5, Opcode::ADD, 4, 1),
        }),
    };
    g4.RunPass<Peephole>();
    // g4.Dump();
    bb = g4.GetBasicBlocks()[0];
    ASSERT_EQ(bb->GetSize(), 3);
    ASSERT_EQ(g4.GetInstById(3), nullptr);
    ASSERT_EQ(g4.GetInstById(4), nullptr);
    CheckUsers(bb, {{5}, {5}, {-1}});
    CheckInstsWithTwoInputs(bb, {{2, 1}});

    // case 5
    Graph g5 = GRAPH{
        BASIC_BLOCK<1>({
            INST(1, Opcode::PARAMETER),
            INST(2, Opcode::CONSTANT, 8),
            INST(3, Opcode::CONSTANT, 16),
            INST(4, Opcode::SUB, 1, 2),
            INST(5, Opcode::SUB, 4, 3),
            INST(6, Opcode::ADD, 5, 4),
        }),
    };
    g5.RunPass<Peephole>();
    // g5.Dump();
    bb = g5.GetBasicBlocks()[0];
    int32_t new_inst_id = static_cast<int32_t>(bb->GetFirstInst()->GetId());
    ASSERT_EQ(bb->GetFirstInst()->GetConstant(), 24);
    ASSERT_EQ(bb->GetSize(), 6);
    ASSERT_EQ(g5.GetInstById(3), nullptr);
    ASSERT_NE(g5.GetInstById(new_inst_id), nullptr);
    CheckUsers(bb, {{5}, {4}, {4}, {6}, {6}, {-1}});
    CheckInstsWithTwoInputs(bb, {{1, 2}, {1, new_inst_id}, {5, 4}});
}

TEST(PEEPHOLE_TEST, TEST_SHR) {
    // case 1
    Graph g1 = GRAPH{
        BASIC_BLOCK<1>({
            INST(1, Opcode::PARAMETER),
            INST(2, Opcode::CONSTANT, 0),
            INST(3, Opcode::CONSTANT, 4),
            INST(4, Opcode::SHR, 1, 2),
            INST(5, Opcode::SHR, 1, 3),
            INST(6, Opcode::ADD, 4, 5),
        }),
    };
    g1.RunPass<Peephole>();
    // g1.Dump();
    auto bb = g1.GetBasicBlocks()[0];
    ASSERT_EQ(bb->GetSize(), 4);
    ASSERT_EQ(g1.GetInstById(2), nullptr);
    ASSERT_EQ(g1.GetInstById(4), nullptr);
    CheckUsers(bb, {{5, 6}, {5}, {6}, {-1}});
    CheckInstsWithTwoInputs(bb, {{1, 3}, {1, 5}});

    // case 2
    Graph g2 = GRAPH{
        BASIC_BLOCK<1>({
            INST(1, Opcode::PARAMETER),
            INST(2, Opcode::PARAMETER),
            INST(3, Opcode::SHL, 1, 2),
            INST(4, Opcode::SHR, 3, 2),
            INST(5, Opcode::ADD, 3, 4),
        }),
    };
    g2.RunPass<Peephole>();
    // g2.Dump();
    bb = g2.GetBasicBlocks()[0];
    ASSERT_EQ(bb->GetSize(), 4);
    ASSERT_EQ(g2.GetInstById(4), nullptr);
    CheckUsers(bb, {{3, 5}, {3}, {5}, {-1}});
    CheckInstsWithTwoInputs(bb, {{1, 2}, {3, 1}});

    // case 3
    Graph g3 = GRAPH{
        BASIC_BLOCK<1>({
            INST(1, Opcode::PARAMETER),
            INST(2, Opcode::CONSTANT, 8),
            INST(3, Opcode::CONSTANT, 16),
            INST(4, Opcode::SHR, 1, 2),
            INST(5, Opcode::SHR, 4, 3),
            INST(6, Opcode::ADD, 5, 4),
        }),
    };
    g3.RunPass<Peephole>();
    // g3.Dump();
    bb = g3.GetBasicBlocks()[0];
    int32_t new_inst_id = static_cast<int32_t>(bb->GetFirstInst()->GetId());
    ASSERT_EQ(bb->GetFirstInst()->GetConstant(), 24);
    ASSERT_EQ(bb->GetSize(), 6);
    ASSERT_EQ(g3.GetInstById(3), nullptr);
    ASSERT_NE(g3.GetInstById(new_inst_id), nullptr);
    CheckUsers(bb, {{5}, {4}, {4}, {6}, {6}, {-1}});
    CheckInstsWithTwoInputs(bb, {{1, 2}, {1, new_inst_id}, {5, 4}});
}

TEST(PEEPHOLE_TEST, TEST_XOR) {
    // case 1
    Graph g1 = GRAPH{
        BASIC_BLOCK<1>({
            INST(1, Opcode::PARAMETER),
            INST(2, Opcode::CONSTANT, 0),
            INST(3, Opcode::CONSTANT, 4),
            INST(4, Opcode::XOR, 1, 2),
            INST(5, Opcode::XOR, 1, 3),
            INST(6, Opcode::ADD, 4, 5),
        }),
    };
    g1.RunPass<Peephole>();
    // g1.Dump();
    auto bb = g1.GetBasicBlocks()[0];
    ASSERT_EQ(bb->GetSize(), 4);
    ASSERT_EQ(g1.GetInstById(2), nullptr);
    ASSERT_EQ(g1.GetInstById(4), nullptr);
    CheckUsers(bb, {{5, 6}, {5}, {6}, {-1}});
    CheckInstsWithTwoInputs(bb, {{1, 3}, {1, 5}});

    // case 2 + case 1
    Graph g2 = GRAPH{
        BASIC_BLOCK<1>({
            INST(1, Opcode::PARAMETER),
            INST(2, Opcode::CONSTANT, 32),
            INST(3, Opcode::SUB, 1, 1),
            INST(4, Opcode::SUB, 1, 3),
            INST(5, Opcode::ADD, 1, 2),
        }),
    };
    g2.RunPass<Peephole>();
    // g2.Dump();
    bb = g2.GetBasicBlocks()[0];
    ASSERT_EQ(bb->GetSize(), 3);
    ASSERT_EQ(g2.GetInstById(3), nullptr);
    ASSERT_EQ(g2.GetInstById(4), nullptr);
    CheckUsers(bb, {{5}, {5}, {-1}});
    CheckInstsWithTwoInputs(bb, {{1, 2}});

    // case 3
    Graph g3 = GRAPH{
        BASIC_BLOCK<1>({
            INST(1, Opcode::PARAMETER),
            INST(2, Opcode::CONSTANT, -1),
            INST(3, Opcode::XOR, 1, 2),
            INST(4, Opcode::ADD, 1, 3),
        }),
    };
    g3.RunPass<Peephole>();
    // g3.Dump();
    bb = g3.GetBasicBlocks()[0];
    int32_t new_inst_id = static_cast<int32_t>(g3.GetInstById(4)->GetPrev()->GetId());
    ASSERT_EQ(bb->GetSize(), 3);
    ASSERT_EQ(g3.GetInstById(2), nullptr);
    ASSERT_EQ(g3.GetInstById(3), nullptr);
    ASSERT_NE(g3.GetInstById(new_inst_id), nullptr);
    CheckUsers(bb, {{4, new_inst_id}, {4}, {-1}});
    CheckInstsWithTwoInputs(bb, {{1, new_inst_id}});
}
