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

class Inst;
class BasicBlock;
class Graph;

#define FORWARD_DECLARATION(Type)   \
class Type;

    TYPE_LIST(FORWARD_DECLARATION)
#undef FORWARD_DECLARATION

class Inst : public Markers
{
  public:
    template <Opcode opcode>
    static Inst* InstBuilder(uint32_t ins_id);

    ACCESSOR_MUTATOR(next_, Next, Inst*)
    ACCESSOR_MUTATOR(prev_, Prev, Inst*)
    ACCESSOR_MUTATOR(id_, Id, uint32_t)
    ACCESSOR_MUTATOR(bb_, BB, BasicBlock*)
    ACCESSOR_MUTATOR(opcode_, Opcode, Opcode)
    ACCESSOR_MUTATOR(type_, Type, Type)
    ACCESSOR_MUTATOR(users_, Users, const std::vector<Inst*>&)

    bool IsStartInst();
    bool IsEndInst();
    virtual void Dump();
    virtual void SubstituteInput(Inst* old_input, Inst* new_input)
    {}

    // TODO rule of 5?
    ~Inst();

    static uint32_t NextId()
    {
        return next_id_;
    }

    void AddUser(Inst* user)
    {
        if (std::find(users_.begin(), users_.end(), user) == users_.end()) {
            users_.push_back(user);
        }
    }

    void RemoveUser(Inst* user)
    {
        auto it = std::find(users_.begin(), users_.end(), user);
        *it = users_.back();
        users_.pop_back();
    }

#define CAST_DECLARE_METHOD(Type)                                                   \
    Type* CastTo##Type();                                                    \

    TYPE_LIST(CAST_DECLARE_METHOD)
#undef CAST_METHOD

  protected:
    Inst() = default;
    Inst(Inst& inst) = default;
    Inst(uint32_t id, Opcode op, Type type) : id_(id), opcode_(op), type_(type)
    {
    }

    void DumpUsers()
    {
        std::cout << "( ";
        for (auto item: users_) {
            std::cout << item->GetId() << " ";
        }
        std::cout << ")";
    }

  private:
    uint32_t id_ = 0;
    BasicBlock* bb_ = nullptr;
    Opcode opcode_ = Opcode::DEFAULT;
    Type type_ = Type::DEFAULT;

    // TODO change to custom list as soon as allocator will be done
    std::vector<Inst*> users_;
    Inst* next_ = nullptr;
    Inst* prev_ = nullptr;

    static inline uint32_t next_id_ = 0;
};

class InstWithTwoInputs : public Inst
{
  public:
    InstWithTwoInputs(uint32_t id, Opcode opcode) : Inst(id, opcode, Type::InstWithTwoInputs) {}

    ACCESSOR_MUTATOR(input1_, Input1, Inst*)
    ACCESSOR_MUTATOR(input2_, Input2, Inst*)

    void Dump() override;
    void SubstituteInput(Inst* old_input, Inst* new_input) override;

  private:
    Inst* input1_ = nullptr;
    Inst* input2_ = nullptr;
};

class InstWithOneInput : public Inst
{
  public:
    InstWithOneInput(uint32_t id, Opcode opcode) : Inst(id, opcode, Type::InstWithOneInput) {}

    ACCESSOR_MUTATOR(input1_, Input1, Inst*)

    void Dump() override;
    void SubstituteInput(Inst* old_input, Inst* new_input) override;

  private:
    Inst* input1_ = nullptr;
};

class InstWithNoInputs : public Inst
{
  public:
    InstWithNoInputs(uint32_t id, Opcode opcode) : Inst(id, opcode, Type::InstWithNoInputs)
    {}
    
    void Dump() override;
};

class InstJmp : public Inst
{
  public:
    InstJmp(uint32_t id, Opcode opcodes) : Inst(id, opcodes, Type::InstJmp) {}

    ACCESSOR_MUTATOR(target_bb_, TargetBB, BasicBlock*)

    void Dump() override;

  private:
    BasicBlock* target_bb_ = nullptr;
};

class InstPhi : public Inst
{
  public:
    InstPhi(uint32_t id, Opcode opcode) : Inst(id, opcode, Type::InstPhi) {}

    ACCESSOR_MUTATOR(input_inst_, InputInst, const std::vector<Inst*>&)
    ACCESSOR_MUTATOR(input_bb_, InputBB, const std::vector<BasicBlock*>&)

    void AddInput(Inst* inst, BasicBlock* bb)
    {
        if (std::find(input_inst_.begin(), input_inst_.end(), inst) == input_inst_.end()) {
            input_inst_.push_back(inst);
            input_bb_.push_back(bb);
        }
    }

    void RemoveInput(Inst* inst)
    {
        auto it = std::find(input_inst_.begin(), input_inst_.end(), inst);
        *it = input_inst_.back();
        input_inst_.pop_back();

        size_t index = std::distance(input_inst_.begin(), it);
        input_bb_[index] = input_bb_.back();
        input_bb_.pop_back();
    }

    void Dump() override;
    void SubstituteInput(Inst* old_input, Inst* new_input) override;

  private:
    std::vector<Inst*> input_inst_;
    std::vector<BasicBlock*> input_bb_;
};

class InstCall : public Inst
{
public:
    InstCall(uint32_t id, Opcode opcode) : Inst(id, opcode, Type::InstCall)
    {}

    ACCESSOR_MUTATOR(callee_, Callee, Graph*)
    ACCESSOR_MUTATOR(arguments_, Arguments, std::vector<Inst*>)

    void Dump() override;
private:
    Graph* callee_;
    std::vector<Inst*> arguments_;
};

class InstConstant : public Inst
{
  public:
    InstConstant(uint32_t id, Opcode opcode) : Inst(id, opcode, Type::InstConstant)
    {}

    ACCESSOR_MUTATOR(constant_, Constant, int32_t)

    void Dump() override;

  private:
    int32_t constant_ = 0;
};

template <Opcode opcode>
Inst* Inst::InstBuilder(uint32_t ins_id)
{
    if (next_id_ < ins_id)
        next_id_ = ins_id;
    next_id_++;

#define BUILD_INST(name, type)                                                         \
    if constexpr (opcode == Opcode::name) {                                          \
        return static_cast<Inst*>(new type(ins_id, opcode));       \
    }

    OPCODE_LIST(BUILD_INST)
#undef BUILD_INST
    UNREACHABLE()
    return nullptr;
}

#endif // INST_H
