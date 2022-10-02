#include "inst.h"

void Inst::throw_error(Opcode op_in, std::string msg) {
    #define ERROR_INST(name, type)                                  \
    if (Opcode::name == op_in) {                                    \
        std::cerr << msg << #name << std::endl;                     \
        std::abort();                                               \
    }
    OPCODE_LIST(ERROR_INST)
    #undef ERROR_INST
}

uint32_t InstNode::GetId()
{
    size_t list_offset = offsetof(Inst, list);
    size_t id_offset = offsetof(Inst, id);
    return *(reinterpret_cast<uint32_t*>(reinterpret_cast<void*>(this) - list_offset + id_offset));
}

Opcode InstNode::GetOpcode()
{
    size_t list_offset = offsetof(Inst, list);
    size_t opcode_offset = offsetof(Inst, op);
    return *(reinterpret_cast<Opcode*>(reinterpret_cast<void*>(this) - list_offset + opcode_offset));
}

BasicBlock *InstNode::GetBasicBlock()
{
    size_t list_offset = offsetof(Inst, list);
    size_t basic_block_offset = offsetof(Inst, bb);
    return *(reinterpret_cast<BasicBlock**>(reinterpret_cast<void*>(this) - list_offset + basic_block_offset));
}

uint8_t InstNode::GetDstReg()
{
    // size_t list_offset = offsetof(Inst, list);
    // return reinterpret_cast<InstBinOp*>(reinterpret_cast<void*>(this) - list_offset)->GetDstReg();
    return 1;
}

// uint8_t InstNode::GetSrcReg1();
// uint8_t InstNode::GetSrcReg2();
// uint8_t InstNode::GetBbId();
// uint8_t InstNode::GetRetValReg();
// uint32_t InstNode::GetImm();