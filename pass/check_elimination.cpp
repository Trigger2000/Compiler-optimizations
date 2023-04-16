#include "check_elimination.h"
#include "dom_tree_fast.h"

void CheckElimination::RunPassImpl(Graph *g)
{
    g->RunPass<DomTreeFast>();

    auto bbs = g->GetBasicBlocks();
    for (BasicBlock* bb: bbs) {
        for (Inst *inst = bb->GetFirstInst(); inst != nullptr; inst = inst->GetNext()) {
            if (inst->GetOpcode() == Opcode::CHECK_EQ_ZERO) {
                TryEliminateCheckOneInput(inst, g);
            } else if (inst->GetOpcode() == Opcode::CHECK_EQ) {
                TryEliminateCheckTwoInput(inst, g);
            }
        }
    }
}

void CheckElimination::TryEliminateCheckOneInput(Inst* inst, Graph *g)
{
    Inst* input = inst->CastToInstWithOneInput()->GetInput1();
    for (auto input_user: input->GetUsers()) {
        if (input_user->GetOpcode() == inst->GetOpcode() && input_user != inst &&
            g->CheckDominance(input_user, inst)) {
            input->RemoveUser(inst);
            delete inst;
        }
    }
}

void CheckElimination::TryEliminateCheckTwoInput(Inst* inst, Graph *g)
{
    Inst* input1 = inst->CastToInstWithTwoInputs()->GetInput1();
    Inst* input2 = inst->CastToInstWithTwoInputs()->GetInput2();
    for (auto input_user: input1->GetUsers()) {
        if (input_user->GetOpcode() == inst->GetOpcode() && input_user != inst &&
            CheckInputsEqual(input_user->CastToInstWithTwoInputs(), inst->CastToInstWithTwoInputs()) &&
            g->CheckDominance(input_user, inst)) {
            input1->RemoveUser(inst);
            input2->RemoveUser(inst);
            delete inst;
        }
    }
}

// works only with equal instruction, maybe remove it
bool CheckElimination::CheckInputsEqual(InstWithTwoInputs* inst1, InstWithTwoInputs* inst2)
{
    return ((inst1->GetInput1() == inst2->GetInput1()) && (inst1->GetInput2() == inst2->GetInput2())) ||
           ((inst1->GetInput2() == inst2->GetInput1()) && (inst1->GetInput1() == inst2->GetInput2()));
}