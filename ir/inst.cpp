#include "inst.h"
#include "basic_block.h"
#include "graph.h"

void Inst::Dump()
{
    if (bb_ != nullptr) {
        std::cout << bb_->GetId();
    } else {
        std::cout << "NO_BB";
    }
    std::cout << ":" << id_ << " ";

#define PRINT_OPCODE(name, type)                                                                                       \
    case Opcode::name:                                                                                                 \
        std::cout << #name << " ";                                                                                     \
        return;

    switch (opcode_) {
        OPCODE_LIST(PRINT_OPCODE)
    }
#undef PRINT_OPCODE
}

Inst::~Inst()
{
    if (GetPrev() != nullptr)
        GetPrev()->SetNext(GetNext());
    if (GetNext() != nullptr)
        GetNext()->SetPrev(GetPrev());
    if (GetBB() != nullptr) {
        if (GetBB()->GetFirstInst() == this)
            GetBB()->SetFirstInst(GetNext());
        if (GetBB()->GetLastInst() == this)
            GetBB()->SetLastInst(GetPrev());
    }
}

bool Inst::IsStartInst()
{
    assert(bb_ != nullptr);
    return bb_->GetFirstInst() == this;
}

bool Inst::IsEndInst()
{
    assert(bb_ != nullptr);
    return bb_->GetLastInst() == this;
}

void InstUsers::Dump()
{
    for (auto item : users_) {
        std::cout << item->GetId() << " ";
    }
}

void InstWithTwoInputs::Dump()
{
    Inst::Dump();
    std::cout << input1_->GetInputId() << " " << input2_->GetInputId() << " -> ";
    users_.Dump();
    std::cout << "\n";
}

void InstWithOneInput::Dump()
{
    Inst::Dump();
    std::cout << input1_->GetInputId() << " -> ";
    users_.Dump();
    std::cout << "\n";
}

void InstJmp::Dump()
{
    Inst::Dump();
    std::cout << target_inst_->GetInputId() << "\n";
}

void InstPhi::Dump()
{
    Inst::Dump();
    for (auto item : phi_inputs_) {
        std::cout << "(" << item->GetInputInst()->GetId() << ", " << item->GetInputBB()->GetId() << ") ";
    }
    std::cout << "-> ";
    users_.Dump();
    std::cout << "\n";
}

void InstParameter::Dump()
{
    Inst::Dump();
    std::cout << "-> ";
    users_.Dump();
    std::cout << "\n";
}

void InstConstant::Dump()
{
    Inst::Dump();
    std::cout << constant_ << " -> ";
    users_.Dump();
    std::cout << "\n";
}

void InstCall::Dump()
{
    Inst::Dump();
    std::cout << callee_->GetName() << " (";
    for (auto arg = arguments_.begin(); arg != std::prev(arguments_.end(), 1); arg = std::next(arg, 1)) {
        std::cout << (*arg)->GetInputId() << ", ";
    }
    std::cout << (*std::prev(arguments_.end(), 1))->GetInputId() << ")\n";
}

void InstWithNoInputsUsers::Dump()
{
    Inst::Dump();
    std::cout << "\n";
}

void PhiInput::SetInputBB(BasicBlock* input_bb)
{
    assert(input_bb->GetId() == input_bb_id_);
    input_bb_ = input_bb;
}

void InstWithTwoInputs::SubstituteInput(Inst* old_input, Inst* new_input)
{
    assert(GetInput1()->GetInputInst() == old_input || GetInput2()->GetInputInst() == old_input);
    if (GetInput1()->GetInputInst() == old_input) {
        GetInput1()->SetInputId(new_input->GetId());
        GetInput1()->SetInputInst(new_input);
    } else {
        GetInput2()->SetInputId(new_input->GetId());
        GetInput2()->SetInputInst(new_input);
    }
}

void InstWithOneInput::SubstituteInput(Inst* old_input, Inst* new_input)
{
    assert(GetInput1()->GetInputInst() == old_input);
    GetInput1()->SetInputId(new_input->GetId());
    GetInput1()->SetInputInst(new_input);
}

void InstPhi::SubstituteInput(Inst* old_input, Inst* new_input)
{
    for (auto phi_input: GetPhiInputs()) {
        if (phi_input->GetInputInst() == old_input) {
            phi_input->SetInputId(new_input->GetId());
            phi_input->SetInputInst(new_input);
            phi_input->SetInputBB(new_input->GetBB());
            phi_input->SetInputBBId(new_input->GetBB()->GetId());
            return;
        }
    }
    UNREACHABLE()
}
