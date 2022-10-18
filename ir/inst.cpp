#include "inst.h"
#include "basic_block.h"
// #include "basic_block.h"

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

void Inst::InstDestroyer(Inst* inst)
{
    assert(inst != nullptr);

#define DESTROY_INST(type)                                                                                             \
    case Type::type:                                                                                                   \
        delete static_cast<type*>(inst);                                                                               \
        break;

    switch (inst->GetType()) {
        TYPE_LIST(DESTROY_INST)
    }

#undef DESTROY_INST
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
        std::cout << "(" << item->GetInputId() << ", " << item->GetInputBBId() << ") ";
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

void PhiInput::SetInputBB(BasicBlock* input_bb)
{
    assert(input_bb->GetId() == input_bb_id_);
    input_bb_ = input_bb;
}