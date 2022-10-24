#ifndef INST_H
#define INST_H

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <list>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "opcode.h"
#include "utils.h"

class InstInput;
class JmpInput;
class PhiInput;
class InstUsers;
class BasicBlock;

// base class for all instructions
class Inst
{
  public:
    template <typename... inputs>
    static Inst* InstBuilder(uint32_t ins_id, const Opcode op_in, inputs... in_range);
    static void InstDestroyer(Inst* inst);

    ACCESSOR_MUTATOR(next_, Next, Inst*)
    ACCESSOR_MUTATOR(prev_, Prev, Inst*)
    ACCESSOR_MUTATOR(id_, Id, uint32_t)
    ACCESSOR_MUTATOR(bb_, BB, BasicBlock*)
    ACCESSOR_MUTATOR(opcode_, Opcode, Opcode)
    ACCESSOR_MUTATOR(type_, Type, Type)

    // TODO change to CRTP
    ACCESSOR_MUTATOR_VIRTUAL(input1_, Input1, InstInput*)
    ACCESSOR_MUTATOR_VIRTUAL(input2_, Input2, InstInput*)
    ACCESSOR_MUTATOR_VIRTUAL(target_inst_, TargetInst, JmpInput*)
    ACCESSOR_MUTATOR_VIRTUAL(constant_, Constant, uint32_t)
    ACCESSOR_MUTATOR_VIRTUAL(phi_inputs_, PhiInputs, std::list<PhiInput*>&)
    ACCESSOR_MUTATOR_VIRTUAL(users_, Users, InstUsers&)

    bool IsStartInst();
    bool IsEndInst();
    virtual void Dump();

  protected:
    Inst() = default;
    Inst(Inst& inst) = default;
    Inst(uint32_t id, Opcode op, Type type) : id_(id), opcode_(op), type_(type)
    {
    }

  private:
    uint32_t id_ = 0;
    BasicBlock* bb_ = nullptr;
    Opcode opcode_ = Opcode::DEFAULT;
    Type type_ = Type::DEFAULT;

    Inst* next_ = nullptr;
    Inst* prev_ = nullptr;
};

// user's definition happens in Graph while building DFG
class InstUsers
{
  public:
    const std::list<Inst*>& GetUsers()
    {
        return users_;
    }

    void AddUser(Inst* user)
    {
        users_.push_back(user);
    }

    void RemoveUser(Inst* user)
    {
        users_.remove(user);
    }

    void Dump();

  private:
    // maybe use custom one?
    std::list<Inst*> users_;
};

// input's definition happens in Graph while building DFG
class InstInput
{
  public:
    InstInput(uint32_t input_id) : input_id_(input_id)
    {
    }

    ACCESSOR_MUTATOR(input_id_, InputId, uint32_t)

    // TODO change to CRTP
    virtual void SetInputInst(Inst* input_inst)
    {
        assert(input_inst->GetId() == input_id_);
        input_inst_ = input_inst;
    }

    Inst* GetInputInst()
    {
        return input_inst_;
    }

  protected:
    uint32_t input_id_ = 0;
    Inst* input_inst_ = nullptr;
};

class JmpInput : public InstInput
{
  public:
    JmpInput(uint32_t input_id) : InstInput(input_id)
    {
    }

    void SetInputInst(Inst* input_inst) override
    {
        assert(input_inst->GetId() == input_id_);
        assert(input_inst->IsStartInst());
        input_inst_ = input_inst;
    }
};

class PhiInput : public InstInput
{
  public:
    PhiInput(uint32_t input_id, uint32_t input_bb_id) : InstInput(input_id), input_bb_id_(input_bb_id)
    {
    }

    ACCESSOR_MUTATOR(input_bb_id_, InputBBId, uint32_t)

    void SetInputBB(BasicBlock* input_bb);

    BasicBlock* GetInputBB()
    {
        return input_bb_;
    }

  private:
    uint32_t input_bb_id_ = 0;
    BasicBlock* input_bb_ = nullptr;
};

class InstWithTwoInputs : public Inst
{
  public:
    template <bool is_phi = false, typename... inputs>
    static InstWithTwoInputs* CreateInst(uint32_t id, Opcode op, inputs... inps);

    ACCESSOR_MUTATOR_OVERRIDE(input1_, Input1, InstInput*)
    ACCESSOR_MUTATOR_OVERRIDE(input2_, Input2, InstInput*)
    ACCESSOR_MUTATOR_OVERRIDE(users_, Users, InstUsers&)

    void Dump() override;

    ~InstWithTwoInputs()
    {
        delete input1_;
        delete input2_;
    }

  private:
    InstWithTwoInputs(uint32_t id, Opcode op, Type type, uint32_t input1, uint32_t input2) : Inst(id, op, type)
    {
        input1_ = new InstInput(input1);
        input2_ = new InstInput(input2);
    }

    //  TODO temporary workaround
    InstWithTwoInputs(uint32_t id, Opcode op, Type type, std::initializer_list<uint32_t> inps)
    {
        UNREACHABLE();
    }

    static const uint32_t INPUTS_COUNT = 2;
    InstInput* input1_ = nullptr;
    InstInput* input2_ = nullptr;
    InstUsers users_;
};

class InstWithOneInput : public Inst
{
  public:
    template <bool is_phi = false, typename... inputs>
    static InstWithOneInput* CreateInst(uint32_t id, Opcode op, inputs... inps);

    ACCESSOR_MUTATOR_OVERRIDE(input1_, Input1, InstInput*)
    ACCESSOR_MUTATOR_OVERRIDE(users_, Users, InstUsers&)

    void Dump() override;

    ~InstWithOneInput()
    {
        delete input1_;
    }

  private:
    InstWithOneInput(uint32_t id, Opcode op, Type type, uint32_t input1) : Inst(id, op, type)
    {
        input1_ = new InstInput(input1);
    }

    //  TODO temporary workaround
    InstWithOneInput(uint32_t id, Opcode op, Type type, std::initializer_list<uint32_t> inps)
    {
        UNREACHABLE()
    }

    static const uint32_t INPUTS_COUNT = 1;
    InstInput* input1_ = nullptr;
    InstUsers users_;
};

class InstJmp : public Inst
{
  public:
    template <bool is_phi = false, typename... inputs>
    static InstJmp* CreateInst(uint32_t id, Opcode op, inputs... inps);

    ACCESSOR_MUTATOR_OVERRIDE(target_inst_, TargetInst, JmpInput*)

    void Dump() override;

    ~InstJmp()
    {
        delete target_inst_;
    }

  private:
    InstJmp(uint32_t id, Opcode op, Type type, uint32_t target_inst_id) : Inst(id, op, type)
    {
        target_inst_ = new JmpInput(target_inst_id);
    }

    //  TODO temporary workaround
    InstJmp(uint32_t id, Opcode op, Type type, std::initializer_list<uint32_t> inps)
    {
        UNREACHABLE()
    }

    static const uint32_t INPUTS_COUNT = 1;
    JmpInput* target_inst_ = nullptr;
};

class InstPhi : public Inst
{
  public:
    template <bool is_phi = true, typename... inputs>
    static InstPhi* CreateInst(uint32_t id, Opcode op, inputs... inps);

    ACCESSOR_MUTATOR_OVERRIDE(phi_inputs_, PhiInputs, std::list<PhiInput*>&)
    ACCESSOR_MUTATOR_OVERRIDE(users_, Users, InstUsers&)

    void Dump() override;

  private:
    InstPhi(uint32_t id, Opcode op, Type type, std::initializer_list<uint32_t> inputs) : Inst(id, op, type)
    {
        if (inputs.size() % 2 != 0) {
            throw_inst_error("Invalid inputs to ", op);
        }
        for (auto item = inputs.begin(); item != inputs.end(); std::advance(item, 2)) {
            phi_inputs_.push_back(new PhiInput(*item, *std::next(item, 1)));
        }
    }

    static const uint32_t INPUTS_COUNT = 0; // variable inputs
    std::list<PhiInput*> phi_inputs_;
    InstUsers users_;
};

class InstParameter : public Inst
{
  public:
    template <bool is_phi = false, typename... inputs>
    static InstParameter* CreateInst(uint32_t id, Opcode op, inputs... inps);

    ACCESSOR_MUTATOR_OVERRIDE(users_, Users, InstUsers&)
    
    void Dump() override;

  private:
    InstParameter(uint32_t id, Opcode op, Type type) : Inst(id, op, type)
    {
    }

    //  TODO temporary workaround
    InstParameter(uint32_t id, Opcode op, Type type, std::initializer_list<uint32_t> inps)
    {
        UNREACHABLE()
    }

    static const uint32_t INPUTS_COUNT = 0;
    InstUsers users_;
};

class InstConstant : public Inst
{
  public:
    template <bool is_phi = false, typename... inputs>
    static InstConstant* CreateInst(uint32_t id, Opcode op, inputs... inps);

    ACCESSOR_MUTATOR_OVERRIDE(constant_, Constant, uint32_t)
    ACCESSOR_MUTATOR_OVERRIDE(users_, Users, InstUsers&)

    void Dump() override;

  private:
    InstConstant(uint32_t id, Opcode op, Type type, uint32_t constant) : Inst(id, op, type), constant_(constant)
    {
    }

    //  TODO temporary workaround
    InstConstant(uint32_t id, Opcode op, Type type, std::initializer_list<uint32_t> inps)
    {
        UNREACHABLE()
    }

    static const uint32_t INPUTS_COUNT = 1;

    uint32_t constant_ = 0;
    InstUsers users_;
};

template <typename... inputs>
Inst* Inst::InstBuilder(uint32_t ins_id, const Opcode op_in, inputs... inputs_range)
{
#define BUILD_INST(name, type)                                                                                         \
    case Opcode::name: {                                                                                               \
        type* new_inst = type::CreateInst(ins_id, op_in, inputs_range...);                                             \
        return static_cast<Inst*>(new_inst);                                                                           \
    }

    switch (op_in) {
        OPCODE_LIST(BUILD_INST)
    }
#undef BUILD_INST

    return nullptr;
}

// TODO compile-time magic, rather hacky...
#define CREATE_INST(type)                                                                                              \
    template <bool is_phi, typename... inputs>                                                                         \
    type* type::CreateInst(uint32_t id, Opcode op, inputs... inps)                                                     \
    {                                                                                                                  \
        type* result = nullptr;                                                                                        \
        /* this statement doesn't works as I want, so to each class constructor with   */                              \
        /* std::initializer_list<uint32_t> should be added */                                                          \
        if constexpr (is_phi) {                                                                                        \
            result = new type(id, op, Type::type, std::initializer_list<uint32_t>{ inps... });                         \
        }                                                                                                              \
        if constexpr (is_phi == false) {                                                                               \
            if constexpr (sizeof...(inps) == type::INPUTS_COUNT) {                                                     \
                result = new type(id, op, Type::type, inps...);                                                        \
            } else {                                                                                                   \
                throw_inst_error("Invalid inputs to ", op);                                                            \
            }                                                                                                          \
        }                                                                                                              \
        return result;                                                                                                 \
    }

TYPE_LIST(CREATE_INST)
#undef CREATE_INST

#endif // INST_H
