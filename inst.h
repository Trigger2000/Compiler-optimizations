#ifndef INST_H
#define INST_H

#include <cstdint>
#include <cstddef>
#include <string>

#include "opcode.h"
#include "basic_block.h"
#include "utils.h"

class InstNode {
public:
    template <typename... inputs>
    static InstNode *InstBuilder(const Opcode op_in, inputs... in_range);

    uint32_t GetId();
    Opcode GetOpcode();
    BasicBlock *GetBasicBlock();
    Type GetType();

    InstNode *GetNext()
    {
        return next;
    }

    InstNode *GetPrev()
    {
        return prev;
    }

    uint8_t GetDstReg();
    uint8_t GetSrcReg1();
    uint8_t GetSrcReg2();
    uint8_t GetBbId();
    uint8_t GetRetValReg();
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

    static void throw_error(Opcode op_in, std::string msg);

private:
    friend class InstNode;

    InstNode list;
    uint32_t id = 0;
    Opcode op = Opcode::DEFAULT;
    Type type = Type::DEFAULT;
    BasicBlock *bb = nullptr;
};


class InstBinOp: public Inst {
public:
    template <typename...inputs>
    static InstBinOp *CreateInst(uint32_t id, Opcode op, inputs... inps);

    uint8_t GetDstReg()
    {
        return dst_reg;
    }

    uint8_t GetSrcReg1()
    {
        return src_reg1;
    }

    uint8_t GetSrcReg2()
    {
        return src_reg2;
    }

private:
    InstBinOp(uint32_t id, Opcode op, Type type, uint8_t dst_reg, uint8_t src_reg1, uint8_t src_reg2):
    Inst(id, op, type), dst_reg(dst_reg), src_reg1(src_reg1), src_reg2(src_reg2) {
        std::cout << "InstBinOp constructed" << std::endl;
    }

    static const uint8_t INPUTS_COUNT = 3;
    uint8_t dst_reg = 0;
    uint8_t src_reg1 = 0;
    uint8_t src_reg2 = 0;
};


class InstBinOpImm: public Inst {
public:
    template <typename...inputs>
    static InstBinOpImm* CreateInst(uint32_t id, Opcode op, inputs... inps);

    uint8_t GetDstReg()
    {
        return dst_reg;
    }

    uint8_t GetSrcReg1()
    {
        return src_reg1;
    }

    uint32_t GetImm()
    {
        return imm;
    }

private:
    InstBinOpImm(uint32_t id, Opcode op, Type type, uint8_t dst_reg, uint8_t src_reg, uint32_t imm):
    Inst(id, op, type), dst_reg(dst_reg), src_reg1(src_reg), imm(imm) {
        std::cout << "InstBinOpImm constructed" << std::endl;
    }

    static const uint8_t INPUTS_COUNT = 3;
    uint8_t dst_reg = 0;
    uint8_t src_reg1 = 0;
    uint32_t imm;
};


class InstControlJmp: public Inst {
public:
    template <typename...inputs>
    static InstControlJmp* CreateInst(uint32_t id, Opcode op, inputs... inps);

    uint8_t GetBbId()
    {
        return bb_id;
    }

private:
    InstControlJmp(uint32_t id, Opcode op, Type type, uint32_t bb_id):
    Inst(id, op, type), bb_id(bb_id) {
        std::cout << "InstControlJmp constructed" << std::endl;
    }

    static const uint8_t INPUTS_COUNT = 1;
    uint8_t bb_id = 0;
    BasicBlock *dst;
};


class InstControlRet: public Inst {
public:
    template <typename...inputs>
    static InstControlRet* CreateInst(uint32_t id, Opcode op, inputs... inps);

    uint8_t GetRetValReg()
    {
        return ret_val_reg;
    }

private:
    InstControlRet(uint32_t id, Opcode op, Type type, uint8_t ret):
    Inst(id, op, type), ret_val_reg(ret) {
        std::cout << "InstControlRet constructed" << std::endl;
    }

    static const uint8_t INPUTS_COUNT = 1;
    uint8_t ret_val_reg = 0;
};


class InstUtil: public Inst {
public:
    template <typename...inputs>
    static InstUtil* CreateInst(uint32_t id, Opcode op, inputs... inps);

    uint8_t GetSrcReg1()
    {
        return src_reg1;
    }

    uint8_t GetSrcReg2()
    {
        return src_reg2;
    }

private:
    InstUtil(uint32_t id, Opcode op, Type type, uint8_t reg1, uint8_t reg2):
    Inst(id, op, type), src_reg1(reg1), src_reg2(reg2) {
        std::cout << "InstUtil constructed" << std::endl;
    }

    static const uint8_t INPUTS_COUNT = 2;
    uint8_t src_reg1 = 0;
    uint8_t src_reg2 = 0;
};


class InstUtilImm: public Inst {
public:
    template <typename...inputs>
    static InstUtilImm* CreateInst(uint32_t id, Opcode op, inputs... inps);

    uint8_t GetSrcReg1()
    {
        return src_reg1;
    }
    
    uint32_t GetImm()
    {
        return imm;
    }

private:
    InstUtilImm(uint32_t id, Opcode op, Type type, uint8_t reg1, uint32_t imm):
    Inst(id, op, type), src_reg1(reg1), imm(imm) {
        // type = Type::InstUtilImm;
        std::cout << "InstUtilImm constructed" << std::endl;
    }

    static const uint8_t INPUTS_COUNT = 2;
    uint8_t src_reg1 = 0;
    uint32_t imm = 0;;
};


template <typename... inputs>
InstNode *InstNode::InstBuilder(const Opcode op_in, inputs... inputs_range) {
    uint32_t new_id = IdCounter::AssignId();

        #define BUILD_INST(name, type)                                          \
        if (Opcode::name == op_in) {                                            \
            type *new_inst = type::CreateInst(new_id, op_in, inputs_range...);  \
            return &new_inst->list;                                             \
        }

        OPCODE_LIST(BUILD_INST)
        #undef BUILD_INST
}

#define CREATE_INST(type)                                                       \
template <typename...inputs>                                                    \
type *type::CreateInst(uint32_t id, Opcode op, inputs... inps) {                \
    if constexpr (sizeof...(inps) == type::INPUTS_COUNT) {                      \
        return new type(id, op, Type::type, inps...);                           \
    }                                                                           \
    throw_error(op, "Invalid inputs to ");                                      \
    UNREACHABLE()                                                               \
}

TYPE_LIST(CREATE_INST)
#undef CREATE_INST

#endif // INST_H