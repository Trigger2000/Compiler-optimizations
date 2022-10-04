#ifndef INST_H
#define INST_H

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <string>
#include <utility>
#include <vector>

#include "opcode.h"
#include "utils.h"

struct PhiInput;

// Implements intrusive list for instructions and corresponding interface
class IListNode
{
  public:
    // TODO remove it to private part?
    IListNode() = default;
    IListNode(IListNode& inst_node) = default;

    template <uint32_t ins_id, typename... inputs>
    static IListNode* InstBuilder(const Opcode op_in, inputs... in_range);
    static void InstDestroyer(IListNode* inst_node);

    IListNode* GetNext()
    {
        return next_;
    }

    IListNode* GetPrev()
    {
        return prev_;
    }

    void SetNext(IListNode* next)
    {
        next_ = next;
    }

    void SetPrev(IListNode* prev)
    {
        prev_ = prev;
    }


    // each of below functions access fields and methods of corresponding classes by offsets

    void SetBBid(uint32_t bb_id);
    void Dump();

    uint32_t GetId();
    uint32_t GetBBId();
    Opcode GetOpcode();
    Type GetType();

    uint32_t GetDstReg();
    uint32_t GetSrcReg1();
    uint32_t GetSrcReg2();
    uint32_t GetBbId();
    uint32_t GetRetValReg();
    uint32_t GetImm();

    const std::vector<PhiInput>& GetPhiInputs();
    const std::vector<uint32_t>& GetInputs();

  private:
    IListNode* next_ = nullptr;
    IListNode* prev_ = nullptr;
};

// base class for all instructions
class Inst
{
  protected:
    Inst() = default;
    Inst(Inst& inst) = default;
    Inst(uint32_t id, Opcode op, Type type) : id(id), op(op), type(type)
    {
    }

    void PrintOpcode();
    static void throw_error(std::string msg, Opcode op_in);

  private:
    // to access inst_node field
    friend class IListNode;

    IListNode inst_node;
    uint32_t id = 0;
    uint32_t bb_id = 0;
    Opcode op = Opcode::DEFAULT;
    Type type = Type::DEFAULT;
};

class InstBinOp : public Inst
{
  public:
    template <bool is_dynamic_args = false, typename... inputs>
    static InstBinOp* CreateInst(uint32_t id, Opcode op, inputs... inps);

    uint32_t GetDstReg()
    {
        return dst_reg;
    }

    uint32_t GetSrcReg1()
    {
        return src_reg1;
    }

    uint32_t GetSrcReg2()
    {
        return src_reg2;
    }

    void Dump()
    {
        PrintOpcode();
        std::cout << dst_reg << " " << src_reg1 << " " << src_reg2 << "\n";
    }

  private:
    InstBinOp(uint32_t id, Opcode op, Type type, uint32_t dst_reg, uint32_t src_reg1, uint32_t src_reg2)
        : Inst(id, op, type), dst_reg(dst_reg), src_reg1(src_reg1), src_reg2(src_reg2)
    {
    }

    //  TODO temporary workaround
    InstBinOp(uint32_t id, Opcode op, Type type, std::initializer_list<uint32_t> inps)
    {
        UNREACHABLE();
    }

    static const uint32_t INPUTS_COUNT = 3;
    uint32_t dst_reg = 0;
    uint32_t src_reg1 = 0;
    uint32_t src_reg2 = 0;
};

class InstBinOpImm : public Inst
{
  public:
    template <bool is_dynamic_args = false, typename... inputs>
    static InstBinOpImm* CreateInst(uint32_t id, Opcode op, inputs... inps);

    uint32_t GetDstReg()
    {
        return dst_reg;
    }

    uint32_t GetSrcReg1()
    {
        return src_reg1;
    }

    uint32_t GetImm()
    {
        return imm;
    }

    void Dump()
    {
        PrintOpcode();
        std::cout << dst_reg << " " << src_reg1 << " " << imm << "\n";
    }

  private:
    InstBinOpImm(uint32_t id, Opcode op, Type type, uint32_t dst_reg, uint32_t src_reg, uint32_t imm)
        : Inst(id, op, type), dst_reg(dst_reg), src_reg1(src_reg), imm(imm)
    {
    }

    //  TODO temporary workaround
    InstBinOpImm(uint32_t id, Opcode op, Type type, std::initializer_list<uint32_t> inps)
    {
        UNREACHABLE()
    }

    static const uint32_t INPUTS_COUNT = 3;
    uint32_t dst_reg = 0;
    uint32_t src_reg1 = 0;
    uint32_t imm;
};

class InstControlJmp : public Inst
{
  public:
    template <bool is_dynamic_args = false, typename... inputs>
    static InstControlJmp* CreateInst(uint32_t id, Opcode op, inputs... inps);

    uint32_t GetBbId()
    {
        return bb_id;
    }

    void Dump()
    {
        PrintOpcode();
        std::cout << bb_id << "\n";
    }

  private:
    InstControlJmp(uint32_t id, Opcode op, Type type, uint32_t bb_id) : Inst(id, op, type), bb_id(bb_id)
    {
    }

    //  TODO temporary workaround
    InstControlJmp(uint32_t id, Opcode op, Type type, std::initializer_list<uint32_t> inps)
    {
        UNREACHABLE()
    }

    static const uint32_t INPUTS_COUNT = 1;
    uint32_t bb_id = 0;
};

class InstControlRet : public Inst
{
  public:
    template <bool is_dynamic_args = false, typename... inputs>
    static InstControlRet* CreateInst(uint32_t id, Opcode op, inputs... inps);

    uint32_t GetRetValReg()
    {
        return ret_val_reg;
    }

    void Dump()
    {
        PrintOpcode();
        std::cout << ret_val_reg << "\n";
    }

  private:
    InstControlRet(uint32_t id, Opcode op, Type type, uint32_t ret) : Inst(id, op, type), ret_val_reg(ret)
    {
    }

    //  TODO temporary workaround
    InstControlRet(uint32_t id, Opcode op, Type type, std::initializer_list<uint32_t> inps)
    {
        UNREACHABLE()
    }

    static const uint32_t INPUTS_COUNT = 1;
    uint32_t ret_val_reg = 0;
};

class InstUtil : public Inst
{
  public:
    template <bool is_dynamic_args = false, typename... inputs>
    static InstUtil* CreateInst(uint32_t id, Opcode op, inputs... inps);

    uint32_t GetSrcReg1()
    {
        return src_reg1;
    }

    uint32_t GetSrcReg2()
    {
        return src_reg2;
    }

    void Dump()
    {
        PrintOpcode();
        std::cout << src_reg1 << " " << src_reg2 << "\n";
    }

  private:
    InstUtil(uint32_t id, Opcode op, Type type, uint32_t reg1, uint32_t reg2)
        : Inst(id, op, type), src_reg1(reg1), src_reg2(reg2)
    {
    }

    //  TODO temporary workaround
    InstUtil(uint32_t id, Opcode op, Type type, std::initializer_list<uint32_t> inps)
    {
        UNREACHABLE()
    }

    static const uint32_t INPUTS_COUNT = 2;
    uint32_t src_reg1 = 0;
    uint32_t src_reg2 = 0;
};

class InstUtilImm : public Inst
{
  public:
    template <bool is_dynamic_args = false, typename... inputs>
    static InstUtilImm* CreateInst(uint32_t id, Opcode op, inputs... inps);

    uint32_t GetSrcReg1()
    {
        return src_reg1;
    }

    uint32_t GetImm()
    {
        return imm;
    }

    void Dump()
    {
        PrintOpcode();
        std::cout << src_reg1 << " " << imm << "\n";
    }

  private:
    InstUtilImm(uint32_t id, Opcode op, Type type, uint32_t reg1, uint32_t imm)
        : Inst(id, op, type), src_reg1(reg1), imm(imm)
    {
    }

    //  TODO temporary workaround
    InstUtilImm(uint32_t id, Opcode op, Type type, std::initializer_list<uint32_t> inps)
    {
        UNREACHABLE()
    }

    static const uint32_t INPUTS_COUNT = 2;
    uint32_t src_reg1 = 0;
    uint32_t imm = 0;
};

struct PhiInput
{
    uint32_t inst_id = 0;
    uint32_t bb_id;
};

class InstPhi : public Inst
{
  public:
    template <bool is_dynamic_args = true, typename... inputs>
    static InstPhi* CreateInst(uint32_t id, Opcode op, inputs... inps);

    const std::vector<PhiInput>& GetPhiInputs()
    {
        return phi_inputs;
    }

    void Dump()
    {
        PrintOpcode();
        std::cout << phi_output << " ";
        for (auto item : phi_inputs) {
            std::cout << "(" << item.inst_id << ", " << item.bb_id << ") ";
        }
        std::cout << "\n";
    }

  private:
    InstPhi(uint32_t id, Opcode op, Type type, std::initializer_list<uint32_t> inputs) : Inst(id, op, type)
    {
        if (inputs.size() % 2 == 0) {
            throw_error("Invalid inputs to ", op);
        }
        phi_output = *inputs.begin();
        for (auto item = std::next(inputs.begin(), 1); item != inputs.end(); std::advance(item, 1)) {
            phi_inputs.push_back({ *item, *std::next(item, 1) });
            std::advance(item, 1);
        }
    }

    static const uint32_t INPUTS_COUNT = 0; // variable inputs
    std::vector<PhiInput> phi_inputs;
    uint32_t phi_output = 0;
};

class InstControlInput : public Inst
{
  public:
    template <bool is_dynamic_args = true, typename... inputs>
    static InstControlInput* CreateInst(uint32_t id, Opcode op, inputs... inps);

    const std::vector<uint32_t>& GetInputs()
    {
        return inputs_;
    }

    void Dump()
    {
        PrintOpcode();
        for (auto item : inputs_) {
            std::cout << item << " ";
        }
        std::cout << "\n";
    }

  private:
    InstControlInput(uint32_t id, Opcode op, Type type, std::initializer_list<uint32_t> inps)
        : Inst(id, op, type), inputs_(inps)
    {
    }

    static const uint32_t INPUTS_COUNT = 0; // variable inputs
    std::vector<uint32_t> inputs_;
};

template <uint32_t ins_id, typename... inputs>
IListNode* IListNode::InstBuilder(const Opcode op_in, inputs... inputs_range)
{
#define BUILD_INST(name, type)                                                                                         \
    if (Opcode::name == op_in) {                                                                                       \
        type* new_inst = type::CreateInst(ins_id, op_in, inputs_range...);                                             \
        return &new_inst->inst_node;                                                                                   \
    }

    OPCODE_LIST(BUILD_INST)
#undef BUILD_INST
    return nullptr;
}

// TODO compile-time magic, rather hacky...
#define CREATE_INST(type)                                                                                              \
    template <bool is_dynamic_args, typename... inputs>                                                                \
    type* type::CreateInst(uint32_t id, Opcode op, inputs... inps)                                                     \
    {                                                                                                                  \
        type* result = nullptr;                                                                                        \
        /* this statement doesn't works as I want, so to each class constructor with   */                              \
        /* std::initializer_list<uint32_t> should be added */                                                          \
        if constexpr (is_dynamic_args) {                                                                               \
            result = new type(id, op, Type::type, std::initializer_list<uint32_t>{ inps... });                         \
        }                                                                                                              \
        if constexpr (is_dynamic_args == false) {                                                                      \
            if constexpr (sizeof...(inps) == type::INPUTS_COUNT) {                                                     \
                result = new type(id, op, Type::type, inps...);                                                        \
            } else {                                                                                                   \
                throw_error("Invalid inputs to ", op);                                                                 \
            }                                                                                                          \
        }                                                                                                              \
        return result;                                                                                                 \
    }

TYPE_LIST(CREATE_INST)
#undef CREATE_INST

#endif // INST_H
