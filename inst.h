#ifndef INST_H
#define INST_H

#include <cstdint>
#include <cstddef>
#include <string>
#include <utility>

#include "opcode.h"
// #include "basic_block.h"
#include "utils.h"

// class BasicBlock;

class InstNode {
public:
    template <typename...inputs>
    static InstNode *InstBuilder(const Opcode op_in, inputs...in_range);

     // TODO variadic non-type template or variadic function?
    static void InstDestroyer(InstNode *inst_node);

    uint32_t GetId();
    Opcode GetOpcode();
    // BasicBlock *GetBasicBlock();
    Type GetType();

    InstNode *GetNext()
    {
        return next;
    }

    InstNode *GetPrev()
    {
        return prev;
    }

    void SetNext(InstNode *next_in)
    {
        next = next_in;
    }

    void SetPrev(InstNode *prev_in)
    {
        prev = prev_in;
    }

    uint32_t GetDstReg();
    uint32_t GetSrcReg1();
    uint32_t GetSrcReg2();
    uint32_t GetBbId();
    uint32_t GetRetValReg();
    uint32_t GetImm();

private:
    friend class Inst;

    InstNode() = default;
    InstNode(InstNode &in) = default;

    InstNode *next = nullptr;
    InstNode *prev = nullptr;
};


class Inst {
protected:
    Inst() = default;
    Inst(Inst &inst) = default;
    Inst(uint32_t id, Opcode op, Type type): id(id), op(op), type(type) {}

    static void throw_error(std::string msg, Opcode op_in);

private:
    friend class InstNode;

    InstNode inst_node;
    uint32_t id = 0;
    Opcode op = Opcode::DEFAULT;
    Type type = Type::DEFAULT;
    // BasicBlock *bb = nullptr;
};


class InstBinOp: public Inst {
public:
    template <typename...inputs>
    static InstBinOp *CreateInst(uint32_t id, Opcode op, inputs... inps);

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

private:
    InstBinOp(uint32_t id, Opcode op, Type type, uint32_t dst_reg, uint32_t src_reg1, uint32_t src_reg2):
    Inst(id, op, type), dst_reg(dst_reg), src_reg1(src_reg1), src_reg2(src_reg2) {}

    static const uint32_t INPUTS_COUNT = 3;
    uint32_t dst_reg = 0;
    uint32_t src_reg1 = 0;
    uint32_t src_reg2 = 0;
};


class InstBinOpImm: public Inst {
public:
    template <typename...inputs>
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

private:
    InstBinOpImm(uint32_t id, Opcode op, Type type, uint32_t dst_reg, uint32_t src_reg, uint32_t imm):
    Inst(id, op, type), dst_reg(dst_reg), src_reg1(src_reg), imm(imm) {}

    static const uint32_t INPUTS_COUNT = 3;
    uint32_t dst_reg = 0;
    uint32_t src_reg1 = 0;
    uint32_t imm;
};


class InstControlJmp: public Inst {
public:
    template <typename...inputs>
    static InstControlJmp* CreateInst(uint32_t id, Opcode op, inputs... inps);

    uint32_t GetBbId()
    {
        return bb_id;
    }

private:
    InstControlJmp(uint32_t id, Opcode op, Type type, uint32_t bb_id):
    Inst(id, op, type), bb_id(bb_id) {}

    static const uint32_t INPUTS_COUNT = 1;
    uint32_t bb_id = 0;
    // BasicBlock *dst;
};


class InstControlRet: public Inst {
public:
    template <typename...inputs>
    static InstControlRet* CreateInst(uint32_t id, Opcode op, inputs... inps);

    uint32_t GetRetValReg()
    {
        return ret_val_reg;
    }

private:
    InstControlRet(uint32_t id, Opcode op, Type type, uint32_t ret):
    Inst(id, op, type), ret_val_reg(ret) {}

    static const uint32_t INPUTS_COUNT = 1;
    uint32_t ret_val_reg = 0;
};


class InstUtil: public Inst {
public:
    template <typename...inputs>
    static InstUtil* CreateInst(uint32_t id, Opcode op, inputs... inps);

    uint32_t GetSrcReg1()
    {
        return src_reg1;
    }

    uint32_t GetSrcReg2()
    {
        return src_reg2;
    }

private:
    InstUtil(uint32_t id, Opcode op, Type type, uint32_t reg1, uint32_t reg2):
    Inst(id, op, type), src_reg1(reg1), src_reg2(reg2) {}

    static const uint32_t INPUTS_COUNT = 2;
    uint32_t src_reg1 = 0;
    uint32_t src_reg2 = 0;
};


class InstUtilImm: public Inst {
public:
    template <typename...inputs>
    static InstUtilImm* CreateInst(uint32_t id, Opcode op, inputs... inps);

    uint32_t GetSrcReg1()
    {
        return src_reg1;
    }
    
    uint32_t GetImm()
    {
        return imm;
    }

private:
    InstUtilImm(uint32_t id, Opcode op, Type type, uint32_t reg1, uint32_t imm):
    Inst(id, op, type), src_reg1(reg1), imm(imm) {}

    static const uint32_t INPUTS_COUNT = 2;
    uint32_t src_reg1 = 0;
    uint32_t imm = 0;;
};


template <typename... inputs>
InstNode *InstNode::InstBuilder(const Opcode op_in, inputs... inputs_range) {
    uint32_t new_id = IdCounter::AssignId();

        #define BUILD_INST(name, type)                                          \
        if (Opcode::name == op_in) {                                            \
            type *new_inst = type::CreateInst(new_id, op_in, inputs_range...);  \
            return &new_inst->inst_node;                                        \
        }

        OPCODE_LIST(BUILD_INST)
        #undef BUILD_INST
}

#define CREATE_INST(type)                                                       \
template <typename ...inputs>                                                   \
type *type::CreateInst(uint32_t id, Opcode op, inputs... inps) {                \
    if constexpr (sizeof...(inps) == type::INPUTS_COUNT) {                      \
        return new type(id, op, Type::type, inps...);                           \
    }                                                                           \
    throw_error("Invalid inputs to ", op);                                      \
    UNREACHABLE()                                                               \
}

TYPE_LIST(CREATE_INST)
#undef CREATE_INST

#endif // INST_H
