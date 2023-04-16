#include "linear_order.h"
#include "loop_analyzer.h"
#include "rpo.h"

void LinearOrder::RunPassImpl(Graph* g)
{
    g->RunPass<RPO>();
    g->RunPass<LoopAnalyzer>();

    auto visit_marker = g->NewMarker();
    for (auto bb: g->GetRPOBasicBlocks()) {
        if (bb->IsMarked(visit_marker)) {
            continue;
        }

        if (bb->IsLoopHeader()) {
            ProccessLoop(bb->GetLoop(), visit_marker);
        } else {
            // in this case it is not needed to inverse jmp
            // instructions since rpo provides correct order
            CheckFalseBranchOrder(bb);

            linear_order_.push_back(bb);
            bb->SetMarker(visit_marker);
        }
    }

    g->SetLinearOrder(linear_order_);
}

void LinearOrder::ProccessLoop(Loop* loop, marker visit_marker)
{
    // reversing, since blocks in loop are stored in reversed order
    for (int i = loop->GetBlocks().size() - 1; i >= 0; --i) {
        auto bb_loop = loop->GetBlocks()[i];
        if (bb_loop->IsLoopHeader() && bb_loop->GetLoop() != loop) {
            ProccessLoop(bb_loop->GetLoop(), visit_marker);
        // second condition is necessary to correctly process bbs from inner loops
        // TODO maybe a little change in loop analysis' logic?
        } else if (!bb_loop->IsMarked(visit_marker) && bb_loop->GetLoop() == loop) {
            // fixing false branch order may be needed
            FixFalseBranchOrder(bb_loop);
            linear_order_.push_back(bb_loop);
            bb_loop->SetMarker(visit_marker);
        }
    }
}

void LinearOrder::FixFalseBranchOrder(BasicBlock* bb)
{
    for (auto pred: bb->GetPreds()) {
        if (pred->GetSuccs().size() == 2) {
            if (pred->GetSize() == 0) {
                continue;
            }

            assert(pred->GetLastInst()->GetType() == Type::InstJmp);

            if (pred->GetSuccs()[BasicBlock::TRUE_BRANCH_INDEX] == bb && linear_order_.back() == pred) {
                pred->GetLastInst()->SetOpcode(GetInverseCondition(pred->GetLastInst()->GetOpcode()));
                pred->SwapSuccs();
            }
        }
    }
}

void LinearOrder::CheckFalseBranchOrder(BasicBlock* bb)
{
    for (auto pred: bb->GetPreds()) {
        if (pred->GetSuccs().size() == 2) {
            if (pred->GetSize() == 0) {
                continue;
            }

            assert(pred->GetLastInst()->GetType() == Type::InstJmp);
            if (pred->GetSuccs()[BasicBlock::FALSE_BRANCH_INDEX] == bb) {
                // false block should follow jmp_<cond> block
                assert(linear_order_.back() == pred);
                // false block should end up with jmp instruction
                assert(bb->GetLastInst()->GetOpcode() == Opcode::JMP);
            }
        }
    }
}

Opcode LinearOrder::GetInverseCondition(Opcode op)
{
    switch (op)
    {
    case Opcode::JMP_EQ:
        return Opcode::JMP_NE;
    case Opcode::JMP_NE:
        return Opcode::JMP_EQ;
    case Opcode::JMP_LE:
        return Opcode::JMP_GT;
    case Opcode::JMP_LT:
        return Opcode::JMP_GE;
    case Opcode::JMP_GE:
        return Opcode::JMP_LT;
    case Opcode::JMP_GT:
        return Opcode::JMP_LE;
    default:
        UNREACHABLE();
        return op;
    }
}
