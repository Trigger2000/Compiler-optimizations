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

void InstWithTwoInputs::Dump()
{
    Inst::Dump();
    std::cout << input1_->GetId() << " " << input2_->GetId() << " -> ";
    Inst::DumpUsers();
    std::cout << "\n";
}

void InstWithOneInput::Dump()
{
    Inst::Dump();
    std::cout << input1_->GetId() << " -> ";
    Inst::DumpUsers();
    std::cout << "\n";
}

void InstJmp::Dump()
{
    Inst::Dump();
    std::cout << target_bb_->GetId() << "\n";
}

void InstPhi::Dump()
{
    Inst::Dump();
    for (int i = 0; i < input_inst_.size(); ++i) {
        std::cout << "(" << input_inst_[i]->GetId() << ", " << input_bb_[i]->GetId() << ") ";
    }
    std::cout << "-> ";
    Inst::DumpUsers();
    std::cout << "\n";
}

void InstConstant::Dump()
{
    Inst::Dump();
    std::cout << constant_ << " -> ";
    Inst::DumpUsers();
    std::cout << "\n";
}

void InstCall::Dump()
{
    Inst::Dump();
    std::cout << "( ";
    for (auto&& item: arguments_) {
        std::cout << item->GetId() << ", ";
    }
    std::cout << ")";
    Inst::DumpUsers();
    std::cout << "\n";
}

void InstWithNoInputs::Dump()
{
    Inst::Dump();
    Inst::DumpUsers();
    std::cout << "\n";
}

void InstWithTwoInputs::SubstituteInput(Inst* old_input, Inst* new_input)
{
    assert(input1_ == old_input || input2_ == old_input);
    if (input1_ == old_input) {
        input1_ = new_input;
    } else {
        input2_ = new_input;
    }
}

void InstWithOneInput::SubstituteInput(Inst* old_input, Inst* new_input)
{
    assert(input1_ == old_input);
    input1_ = new_input;
}

void InstPhi::SubstituteInput(Inst* old_input, Inst* new_input)
{
    for (int i = 0; i < input_inst_.size(); ++i) {
        if (input_inst_[i] == old_input) {
            input_inst_[i] = new_input;
            input_bb_[i] = new_input->GetBB();
            return;
        }
    }
    UNREACHABLE()
}

#define CAST_DEFINE_METHOD(Type)                                        \
Type* Inst::CastTo##Type()                                              \
{                                                                       \
    return static_cast<Type*>(this);                                    \
}

    TYPE_LIST(CAST_DEFINE_METHOD)
#undef CAST_METHOD
