#ifndef INST_H
#define INST_H

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <list>
#include <string>
#include <utility>
#include <vector>
#include <algorithm>

#include "opcode.h"
#include "utils.h"
#include "marker.h"

class InstInput;
class JmpInput;
class PhiInput;
class InstUsers;
class BasicBlock;
class Graph;

// base class for all instructions
// imho, bad design...
class Inst : public Markers
{
  public:
    template <Opcode opcode, typename... inputs>
    static Inst* InstBuilder(uint32_t ins_id, inputs... in_range);

    ACCESSOR_MUTATOR(next_, Next, Inst*)
    ACCESSOR_MUTATOR(prev_, Prev, Inst*)
    ACCESSOR_MUTATOR(id_, Id, uint32_t)
    ACCESSOR_MUTATOR(bb_, BB, BasicBlock*)
    ACCESSOR_MUTATOR(opcode_, Opcode, Opcode)
    ACCESSOR_MUTATOR(type_, Type, Type)

    // TODO change to something else
    ACCESSOR_MUTATOR_VIRTUAL(input1_, Input1, InstInput*)
    ACCESSOR_MUTATOR_VIRTUAL(input2_, Input2, InstInput*)
    ACCESSOR_MUTATOR_VIRTUAL(target_inst_, TargetInst, JmpInput*)
    ACCESSOR_MUTATOR_VIRTUAL(constant_, Constant, int32_t)
    ACCESSOR_MUTATOR_VIRTUAL(phi_inputs_, PhiInputs, std::vector<PhiInput*>&)
    ACCESSOR_MUTATOR_VIRTUAL(arguments_, Arguments, std::vector<InstInput*>&)
    ACCESSOR_MUTATOR_VIRTUAL(users_, Users, InstUsers&)
    ACCESSOR_MUTATOR_VIRTUAL(callee_, Callee, Graph*)

    bool IsStartInst();
    bool IsEndInst();
    virtual void Dump();
    virtual void SubstituteInput(Inst* old_input, Inst* new_input)
    {}

    virtual ~Inst();

    static uint32_t NextId()
    {
        return next_id_;
    }
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

    static inline uint32_t next_id_ = 0;
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
        if (std::find(users_.begin(), users_.end(), user) == users_.end())
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

    InstInput(Inst* input_inst) : input_inst_(input_inst), input_id_(input_inst->GetId())
    {
    }

    ACCESSOR_MUTATOR(input_id_, InputId, uint32_t)

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

    PhiInput(Inst *input_inst, BasicBlock* input_bb) : InstInput(input_inst), input_bb_(input_bb)
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
    template <Opcode opcode, typename... inputs>
    static Inst* CreateInst(uint32_t id, inputs... inputs_range)
    {
        return static_cast<Inst*>(new InstWithTwoInputs(id, opcode, inputs_range...));
    }

    ACCESSOR_MUTATOR_OVERRIDE(input1_, Input1, InstInput*)
    ACCESSOR_MUTATOR_OVERRIDE(input2_, Input2, InstInput*)
    ACCESSOR_MUTATOR_OVERRIDE(users_, Users, InstUsers&)

    void Dump() override;
    void SubstituteInput(Inst* old_input, Inst* new_input) override;

    ~InstWithTwoInputs()
    {
        delete input1_;
        delete input2_;
    }

  private:
  InstWithTwoInputs(uint32_t id, Opcode op, uint32_t input1, uint32_t input2) : Inst(id, op, Type::InstWithTwoInputs)
    {
        input1_ = new InstInput(input1);
        input2_ = new InstInput(input2);
    }

    InstInput* input1_ = nullptr;
    InstInput* input2_ = nullptr;
    InstUsers users_;
};

class InstWithOneInput : public Inst
{
  public:
    template <Opcode opcode, typename... inputs>
    static Inst* CreateInst(uint32_t id, inputs... inputs_range)
    {
        return static_cast<Inst*>(new InstWithOneInput(id, opcode, inputs_range...));
    }

    ACCESSOR_MUTATOR_OVERRIDE(input1_, Input1, InstInput*)
    ACCESSOR_MUTATOR_OVERRIDE(users_, Users, InstUsers&)

    void Dump() override;
    void SubstituteInput(Inst* old_input, Inst* new_input) override;

    ~InstWithOneInput()
    {
        delete input1_;
    }

  private:
  InstWithOneInput(uint32_t id, Opcode op, uint32_t input1) : Inst(id, op, Type::InstWithOneInput)
    {
        input1_ = new InstInput(input1);
    }

    InstInput* input1_ = nullptr;
    InstUsers users_;
};

class InstJmp : public Inst
{
  public:
    template <Opcode opcode, typename... inputs>
    static Inst* CreateInst(uint32_t id, inputs... inputs_range)
    {
        return static_cast<Inst*>(new InstJmp(id, opcode, inputs_range...));
    }

    ACCESSOR_MUTATOR_OVERRIDE(target_inst_, TargetInst, JmpInput*)

    void Dump() override;

    ~InstJmp()
    {
        delete target_inst_;
    }

  private:
    InstJmp(uint32_t id, Opcode op, uint32_t target_inst_id) : Inst(id, op, Type::InstJmp)
    {
        target_inst_ = new JmpInput(target_inst_id);
    }

    // TODO change input to basic block (incorrect inlining)
    JmpInput* target_inst_ = nullptr;
};

class InstPhi : public Inst
{
  public:
    template <Opcode opcode, typename... inputs>
    static Inst* CreateInst(uint32_t id, inputs... inputs_range)
    {
        return static_cast<Inst*>(new InstPhi(id, opcode, {inputs_range...}));
    }

    ACCESSOR_MUTATOR_OVERRIDE(phi_inputs_, PhiInputs, std::vector<PhiInput*>&)
    ACCESSOR_MUTATOR_OVERRIDE(users_, Users, InstUsers&)

    void Dump() override;
    void SubstituteInput(Inst* old_input, Inst* new_input) override;

  private:
  InstPhi(uint32_t id, Opcode op, std::initializer_list<uint32_t> inputs) : Inst(id, op, Type::InstPhi)
    {
        if (inputs.size() % 2 != 0) {
            throw_inst_error("Invalid inputs to ", op);
        }
        for (auto item = inputs.begin(); item != inputs.end(); std::advance(item, 2)) {
            phi_inputs_.push_back(new PhiInput(*item, *std::next(item, 1)));
        }
    }

    std::vector<PhiInput*> phi_inputs_;
    InstUsers users_;
};

class InstCall : public Inst
{
public:
    template <Opcode opcode, typename... inputs>
    static Inst* CreateInst(uint32_t id, inputs... inputs_range)
    {
        return static_cast<Inst*>(new InstCall(id, opcode, inputs_range...));
    }

    ACCESSOR_MUTATOR_OVERRIDE(callee_, Callee, Graph*)
    ACCESSOR_MUTATOR_OVERRIDE(arguments_, Arguments, std::vector<InstInput*>&)
    ACCESSOR_MUTATOR_OVERRIDE(users_, Users, InstUsers&)

    void Dump() override;
private:
    template <typename... inputs>
    InstCall(uint32_t id, Opcode op, Graph* callee, inputs... inputs_range) : Inst(id, op, Type::InstCall), callee_(callee),
    arguments_({new InstInput(inputs_range)...})
    {
    }

    Graph* callee_;
    std::vector<InstInput*> arguments_;
    InstUsers users_;
};

class InstParameter : public Inst
{
  public:
    template <Opcode opcode, typename... inputs>
    static Inst* CreateInst(uint32_t id, inputs... inputs_range)
    {
        return static_cast<Inst*>(new InstParameter(id, opcode, inputs_range...));
    }

    ACCESSOR_MUTATOR_OVERRIDE(users_, Users, InstUsers&)
    
    void Dump() override;

  private:
  InstParameter(uint32_t id, Opcode op) : Inst(id, op, Type::InstParameter)
    {
    }

    InstUsers users_;
};

class InstWithNoInputsUsers : public Inst
{
   public:
    template <Opcode opcode, typename... inputs>
    static Inst* CreateInst(uint32_t id, inputs... inputs_range)
    {
        return static_cast<Inst*>(new InstWithNoInputsUsers(id, opcode, inputs_range...));
    }
    
    void Dump() override;

  private:
  InstWithNoInputsUsers(uint32_t id, Opcode op) : Inst(id, op, Type::InstWithNoInputsUsers)
    {
    }
};

class InstConstant : public Inst
{
  public:
    template <Opcode opcode, typename... inputs>
    static Inst* CreateInst(uint32_t id, inputs... inputs_range)
    {
        return static_cast<Inst*>(new InstConstant(id, opcode, inputs_range...));
    }

    ACCESSOR_MUTATOR_OVERRIDE(constant_, Constant, int32_t)
    ACCESSOR_MUTATOR_OVERRIDE(users_, Users, InstUsers&)

    void Dump() override;

  private:
  InstConstant(uint32_t id, Opcode op, int32_t constant) : Inst(id, op, Type::InstConstant), constant_(constant)
    {
    }

    int32_t constant_ = 0;
    InstUsers users_;
};

template <Opcode opcode, typename... inputs>
Inst* Inst::InstBuilder(uint32_t ins_id, inputs... inputs_range)
{
    if (next_id_ < ins_id)
        next_id_ = ins_id;
    next_id_++;

#define BUILD_INST(name, type)                                                         \
    if constexpr (opcode == Opcode::name) {                                          \
        return type::CreateInst<opcode>(ins_id, inputs_range...);                       \
    }

    OPCODE_LIST(BUILD_INST)
#undef BUILD_INST
    UNREACHABLE()
    return nullptr;
}

#endif // INST_H
