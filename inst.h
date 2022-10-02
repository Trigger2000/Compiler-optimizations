#ifndef INST_H
#define INST_H

#include <cstdint>

#include "opcode.h"
#include "basic_block.h"
#include "utils.h"

class Inst {
public:
    template <typename... inputs>
    static IntrusiveListNode *InstBuilder(const Opcode op_in, inputs... in_range);

    IntrusiveListNode *GetNext()
    {
        return list.next;
    }

    IntrusiveListNode *GetPrev()
    {
        return list.prev;
    }

    void SetNext(IntrusiveListNode *next)
    {
        list.next = next;
    }

    void SetPrev(IntrusiveListNode *prev)
    {
        list.prev = prev;
    }

    Opcode GetOpcode()
    {
        return op;
    }

    BasicBlock *GetBB()
    {
        return bb;
    }

protected:
    Inst() = default;

    static void invalid_inputs(Opcode op_in) {
        #define ERROR_INST(name, type)                                  \
        if (Opcode::name == op_in) {                                    \
            std::cerr << "Invalid inputs to " << #name << std::endl;    \
            std::abort();                                               \
        }

        OPCODE_LIST(ERROR_INST)
        #undef ERROR_INST
    }

    Inst(uint32_t id, Opcode op): id(id), op(op) {}

private:
    uint32_t id = 0;
    Opcode op = Opcode::DEFAULT;
    IntrusiveListNode list;
    BasicBlock *bb = nullptr;
};


class InstBinOp: public Inst {
public:
    template <typename...inputs>
    static InstBinOp *CreateInst(uint32_t id, Opcode op, inputs... inps) {
        if constexpr (sizeof...(inps) == 3) {
            return new InstBinOp(id, op, inps...);
        }
        invalid_inputs(op);
        UNREACHABLE()
    }

private:
    InstBinOp(uint32_t id, Opcode op, uint8_t dst_reg, uint8_t src_reg1, uint8_t src_reg2):
    Inst(id, op), dst_reg(dst_reg), src_reg1(src_reg1), src_reg2(src_reg2) {
        std::cout << "InstBinOp constructed" << std::endl;
    }

    uint8_t dst_reg = 0;
    uint8_t src_reg1 = 0;
    uint8_t src_reg2 = 0;
};


class InstBinOpImm: public Inst {
public:
    template <typename...inputs>
    static InstBinOpImm* CreateInst(uint32_t id, Opcode op, inputs... inps) {
        if constexpr (sizeof...(inps) == 3) {
            return new InstBinOpImm(id, op, inps...);
        }
        invalid_inputs(op);
        UNREACHABLE()
    }

private:
    InstBinOpImm(uint32_t id, Opcode op, uint8_t dst_reg, uint8_t src_reg, uint32_t imm):
    Inst(id, op), dst_reg(dst_reg), src_reg(src_reg), imm(imm) {
        std::cout << "InstBinOpImm constructed" << std::endl;
    }

    uint8_t dst_reg = 0;
    uint8_t src_reg = 0;
    uint32_t imm;
};


class InstControlJmp: public Inst {
public:
    template <typename...inputs>
    static InstControlJmp* CreateInst(uint32_t id, Opcode op, inputs... inps) {
        if constexpr (sizeof...(inps) == 1) {
            return new InstControlJmp(id, op, inps...);
        }
        invalid_inputs(op);
        UNREACHABLE()
    }

private:
    InstControlJmp(uint32_t id, Opcode op, BasicBlock *dst_bb):
    Inst(id, op), dst(dst_bb) {
        std::cout << "InstControlJmp constructed" << std::endl;
    }

    BasicBlock *dst;
};


class InstControlRet: public Inst {
public:
    template <typename...inputs>
    static InstControlRet* CreateInst(uint32_t id, Opcode op, inputs... inps) {
        if constexpr (sizeof...(inps) == 1) {
            return new InstControlRet(id, op, inps...);
        }
        invalid_inputs(op);
        UNREACHABLE()
    }

private:
    InstControlRet(uint32_t id, Opcode op, uint8_t ret):
    Inst(id, op), ret_value_reg(ret) {
        std::cout << "InstControlRet constructed" << std::endl;
    }

    uint8_t ret_value_reg = 0;
};


class InstUtil: public Inst {
public:
    template <typename...inputs>
    static InstUtil* CreateInst(uint32_t id, Opcode op, inputs... inps) {
        if constexpr (sizeof...(inps) == 2) {
            return new InstUtil(id, op, inps...);
        }
        invalid_inputs(op);
        UNREACHABLE()
    }

private:
    InstUtil(uint32_t id, Opcode op, uint8_t reg1, uint8_t reg2):
    Inst(id, op), reg1(reg1), reg2(reg2) {
        std::cout << "InstUtil constructed" << std::endl;
    }

    uint8_t reg1 = 0;
    uint8_t reg2 = 0;
};


class InstUtilImm: public Inst {
public:
    template <typename...inputs>
    static InstUtilImm* CreateInst(uint32_t id, Opcode op, inputs... inps) {
        if constexpr (sizeof...(inps) == 2) {
            return new InstUtilImm(id, op, inps...);
        }
        invalid_inputs(op);
        UNREACHABLE()
    }

private:
    InstUtilImm(uint32_t id, Opcode op, uint8_t reg1, uint32_t imm):
    Inst(id, op), reg1(reg1), imm(imm) {
        std::cout << "InstUtilImm constructed" << std::endl;
    }

    uint8_t reg1 = 0;
    uint32_t imm = 0;;
};


template <typename... inputs>
IntrusiveListNode *Inst::InstBuilder(const Opcode op_in, inputs... inputs_range) {
    uint32_t new_id = IdCounter::AssignId();

        #define BUILD_INST(name, type)                                          \
        if (Opcode::name == op_in) {                                            \
            type *new_inst = type::CreateInst(new_id, op_in, inputs_range...);  \
            return &new_inst->list;                                             \
        }

        OPCODE_LIST(BUILD_INST)
        #undef BUILD_INST
}

#endif // INST_H