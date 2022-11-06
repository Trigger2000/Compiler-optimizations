#include "gtest/gtest.h"

#include "ir/graph.h"

#define INST Inst::InstBuilder
#define BASIC_BLOCK BasicBlock::BasicBlockBuilder
#define GRAPH Graph

TEST(IR_TEST, TEST1) {
    /*
    :1                                          u64 fact(u32 a0)
        1 parameter
        2 constant 1
        3 constant 2      
    :2
        4 mov         2                         movi.u64  v0, 1
        5 mov         3                         movi.u64  v1, 2
        6 cast        1                         u32tou64  v2, a0
    :3                                          loop:
        7 phi         (5, 2) (13, 4)
        8 cmp         7, 6                      cmp.u64   v1, v2
        9 ja          15                        ja done
    :4
        10 phi         (4, 2) (12, 4)
        11 phi         (5, 2) (13, 4)
        12 mul         10, 11                   mul.u64   v0, v0, v1
        13 add         11, 2                    addi.u64  v1, v1, 1
        14 jmp         7                        jmp loop
    :5                                          done:
        15 phi         (4, 2) (12, 4)
        16 ret         15                       ret.u64   v0
    */

   Graph g = GRAPH{
        BASIC_BLOCK<1, 2>({
            INST(1, Opcode::PARAMETER),
            INST(2, Opcode::CONSTANT, 1),
            INST(3, Opcode::CONSTANT, 2),
        }),
        BASIC_BLOCK<2, 3>({
            INST(4, Opcode::MOV, 2),
            INST(5, Opcode::MOV, 3),
            INST(6, Opcode::CAST, 1),
        }),
        BASIC_BLOCK<3, 4, 5>({
            INST(7, Opcode::PHI, 5, 2, 13, 4),
            INST(8, Opcode::CMP, 7, 6),
            INST(9, Opcode::JA, 15)
        }),
        BASIC_BLOCK<4, 3>({
            INST(10, Opcode::PHI, 4, 2, 12, 4),
            INST(11, Opcode::PHI, 5, 2, 13, 4),
            INST(12, Opcode::MUL, 10, 11),
            INST(13, Opcode::ADD, 11, 2),
            INST(14, Opcode::JMP, 7),
        }),
        BASIC_BLOCK<5>({
            INST(15, Opcode::PHI, 4, 2, 12, 4),
            INST(16, Opcode::RET, 15),
        })
    };

    BasicBlock *bb1 = g.GetBBbyId(1);
    BasicBlock *bb2 = g.GetBBbyId(2);
    BasicBlock *bb3 = g.GetBBbyId(3);
    BasicBlock *bb4 = g.GetBBbyId(4);
    BasicBlock *bb5 = g.GetBBbyId(5);

    const std::vector <BasicBlock*>& preds1 = bb1->GetPreds();
    const std::vector <BasicBlock*>& preds2 = bb2->GetPreds();
    const std::vector <BasicBlock*>& preds3 = bb3->GetPreds();
    const std::vector <BasicBlock*>& preds4 = bb4->GetPreds();
    const std::vector <BasicBlock*>& preds5 = bb5->GetPreds();

    assert(preds1.empty());
    assert(preds2.size() == 1);
    assert(preds3.size() == 2);
    assert(preds4.size() == 1);
    assert(preds5.size() == 1);

    assert(preds2[0] == bb1);
    assert(preds3[0] == bb2);
    assert(preds3[1] == bb4);
    assert(preds4[0] == bb3);
    assert(preds5[0] == bb3);

    const std::vector<std::variant<uint32_t, BasicBlock*>>& succs1 = bb1->GetSuccs();
    const std::vector<std::variant<uint32_t, BasicBlock*>>& succs2 = bb2->GetSuccs();
    const std::vector<std::variant<uint32_t, BasicBlock*>>& succs3 = bb3->GetSuccs();
    const std::vector<std::variant<uint32_t, BasicBlock*>>& succs4 = bb4->GetSuccs();
    const std::vector<std::variant<uint32_t, BasicBlock*>>& succs5 = bb5->GetSuccs();

    assert(succs1.size() == 1);
    assert(succs2.size() == 1);
    assert(succs3.size() == 2);
    assert(succs4.size() == 1);
    assert(succs5.empty());

    // TODO check successors here

    Inst *bb1inst1 = bb1->GetFirstInst();
    Inst *bb1inst2 = bb1->GetFirstInst()->GetNext();
    Inst *bb1inst3 = bb1->GetLastInst();

    assert(bb1inst1->GetId() == 1);
    assert(bb1inst2->GetId() == 2);
    assert(bb1inst3->GetId() == 3);

    assert(bb1inst1->GetBB() == bb1);
    assert(bb1inst2->GetBB() == bb1);
    assert(bb1inst3->GetBB() == bb1);

    assert(bb1inst1->GetOpcode() == Opcode::PARAMETER);
    assert(bb1inst2->GetOpcode() == Opcode::CONSTANT);
    assert(bb1inst3->GetOpcode() == Opcode::CONSTANT);

    assert(bb1inst1->GetType() == Type::InstParameter);
    assert(bb1inst2->GetType() == Type::InstConstant);
    assert(bb1inst3->GetType() == Type::InstConstant);

    // TODO finish test
}