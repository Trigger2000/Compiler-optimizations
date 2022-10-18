#include "ir/graph.h"

#define INST Inst::InstBuilder
#define BASIC_BLOCK BasicBlock::BasicBlockBuilder
#define GRAPH Graph

// TODO migrate to googletest
void LectureTest() {
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

   Graph g = GRAPH{
        BASIC_BLOCK<1, 2>({
            INST<1>(Opcode::PARAMETER, 0),
            INST<2>(Opcode::MOVI, 1, 1),
            INST<3>(Opcode::MOVI, 2, 2),
            INST<4>(Opcode::CAST, 3, 0)
        }),
        BASIC_BLOCK<2, 3, 4>({
            INST<1>(Opcode::PHI, 4, 2, 1, 7, 3),
            INST<2>(Opcode::CMP, 4, 3),
            INST<3>(Opcode::JA, 4),
        }),
        BASIC_BLOCK<3, 2, 4>({
            INST<1>(Opcode::PHI, 5, 1, 1, 6, 3),
            INST<2>(Opcode::MUL, 6, 5, 4),
            INST<3>(Opcode::ADDI, 7, 4, 1),
            INST<4>(Opcode::JMP, 2)
        }),
        BASIC_BLOCK<4>({
            INST<1>(Opcode::PHI, 8, 1, 1, 6, 2),
            INST<2>(Opcode::RET, 8),
        })
    };

    g.Dump();

    BasicBlock *bb1 = g.GetBBbyId(1);
    BasicBlock *bb2 = g.GetBBbyId(2);
    BasicBlock *bb3 = g.GetBBbyId(3);
    BasicBlock *bb4 = g.GetBBbyId(4);

    const std::unordered_map<uint32_t, BasicBlock*>& preds1 = bb1->GetPreds();
    const std::unordered_map<uint32_t, BasicBlock*>& preds2 = bb2->GetPreds();
    const std::unordered_map<uint32_t, BasicBlock*>& preds3 = bb3->GetPreds();
    const std::unordered_map<uint32_t, BasicBlock*>& preds4 = bb4->GetPreds();

    assert(preds1.empty());
    assert(preds2.find(1) != preds2.end());
    assert(preds2.find(3) != preds2.end());
    assert(preds3.find(2) != preds3.end());
    assert(preds4.find(2) != preds4.end());
    assert(preds4.find(3) != preds4.end());

    const std::unordered_map<uint32_t, BasicBlock*>& succs1 = bb1->GetSuccs();
    const std::unordered_map<uint32_t, BasicBlock*>& succs2 = bb2->GetSuccs();
    const std::unordered_map<uint32_t, BasicBlock*>& succs3 = bb3->GetSuccs();
    const std::unordered_map<uint32_t, BasicBlock*>& succs4 = bb4->GetSuccs();

    assert(succs1.find(2) != succs1.end());
    assert(succs2.find(3) != succs2.end());
    assert(succs2.find(4) != succs2.end());
    assert(succs3.find(2) != succs3.end());
    assert(succs3.find(4) != succs3.end());
    assert(succs4.empty());


    Inst *bb1inst1 = bb1->GetFirstinst();
    Inst *bb1inst2 = bb1->GetFirstinst()->GetNext();
    Inst *bb1inst3 = bb1->GetLastInst()->GetPrev();
    Inst *bb1inst4 = bb1->GetLastInst();

    assert(bb1inst1->GetId() == 1);
    assert(bb1inst2->GetId() == 2);
    assert(bb1inst3->GetId() == 3);
    assert(bb1inst4->GetId() == 4);

    assert(bb1inst1->GetBBId() == 1);
    assert(bb1inst2->GetBBId() == 1);
    assert(bb1inst3->GetBBId() == 1);
    assert(bb1inst4->GetBBId() == 1);

    assert(bb1inst1->GetOpcode() == Opcode::PARAMETER);
    assert(bb1inst2->GetOpcode() == Opcode::MOVI);
    assert(bb1inst3->GetOpcode() == Opcode::MOVI);
    assert(bb1inst4->GetOpcode() == Opcode::CAST);

    assert(bb1inst1->GetType() == Type::InstParameter);
    // assert(bb1inst2->GetType() == Type::InstWithTwoInputsImm);
    // assert(bb1inst3->GetType() == Type::InstWithTwoInputsImm);
    assert(bb1inst4->GetType() == Type::InstUtil);

    assert(bb1inst1->GetInputs()[0] == 0);
    // assert(bb1inst2->GetDstReg() == 1);
    assert(bb1inst2->GetImm() == 1);
    // assert(bb1inst3->GetDstReg() == 2);
    assert(bb1inst3->GetImm() == 2);
    assert(bb1inst4->GetSrcReg1() == 3);
    assert(bb1inst4->GetSrcReg2() == 0);
}