#include "inst.h"

void Inst::throw_error(std::string msg, Opcode op_in) {
    #define ERROR_INST(name, type)                                  \
    if (Opcode::name == op_in) {                                    \
        std::cerr << msg << #name << std::endl;                     \
        std::abort();                                               \
    }
    OPCODE_LIST(ERROR_INST)
    #undef ERROR_INST
}

void Inst::PrintOpcode() {
    std::cout << bb_id << ":" << id << " ";
    #define PRINT_OPCODE(name, type)                                \
    if (Opcode::name == op) {                                       \
        std::cout << #name << " ";                                  \
        return;                                                     \
    }
    OPCODE_LIST(PRINT_OPCODE)
    #undef PRINT_OPCODE
}

void InstNode::InstDestroyer(InstNode *inst_node)
{
    assert(inst_node != nullptr);
    size_t node_offset = offsetof(Inst, inst_node);

    #define DESTROY_INST(type)                                                              \
    case Type::type:                                                                        \
        delete reinterpret_cast<type*>(reinterpret_cast<void*>(inst_node) - node_offset);   \
        break;

    switch (inst_node->GetType())
    {
        TYPE_LIST(DESTROY_INST)
    }

    #undef DESTROY_INST
}

void InstNode::Dump()
{
    size_t node_offset = offsetof(Inst, inst_node);

    #define DUMP_INST(type)                                                             \
    case Type::type:                                                                    \
        reinterpret_cast<type*>(reinterpret_cast<void*>(this) - node_offset)->Dump();   \
        return;

    switch (GetType())
    {
        TYPE_LIST(DUMP_INST)
    }

    UNREACHABLE()
}

void InstNode::SetBBid(uint32_t bb_id)
{
    size_t node_offset = offsetof(Inst, inst_node);
    size_t bd_id_offset = offsetof(Inst, bb_id);
    *(reinterpret_cast<uint32_t*>(reinterpret_cast<void*>(this) - node_offset + bd_id_offset)) = bb_id;
}

// TODO change getters to macros codegen?

uint32_t InstNode::GetId()
{
    size_t node_offset = offsetof(Inst, inst_node);
    size_t id_offset = offsetof(Inst, id);
    return *(reinterpret_cast<uint32_t*>(reinterpret_cast<void*>(this) - node_offset + id_offset));
}

uint32_t InstNode::GetBBId()
{
    size_t node_offset = offsetof(Inst, inst_node);
    size_t bd_id_offset = offsetof(Inst, bb_id);
    return *(reinterpret_cast<uint32_t*>(reinterpret_cast<void*>(this) - node_offset + bd_id_offset));
}

Opcode InstNode::GetOpcode()
{
    size_t node_offset = offsetof(Inst, inst_node);
    size_t opcode_offset = offsetof(Inst, op);
    return *(reinterpret_cast<Opcode*>(reinterpret_cast<void*>(this) - node_offset + opcode_offset));
}

Type InstNode::GetType()
{
    size_t node_offset = offsetof(Inst, inst_node);
    size_t type_offset = offsetof(Inst, type);
    return *(reinterpret_cast<Type*>(reinterpret_cast<void*>(this) - node_offset + type_offset));
}

uint32_t InstNode::GetDstReg()
{
    Type type = GetType();
    if (type != Type::InstBinOp && type != Type::InstBinOpImm) {
        Inst::throw_error("No DstReg in ", GetOpcode());
    }
    size_t node_offset = offsetof(Inst, inst_node);
    switch (type)
    {
    case Type::InstBinOp:
        return reinterpret_cast<InstBinOp*>(reinterpret_cast<void*>(this) - node_offset)->GetDstReg();
        break;
    case Type::InstBinOpImm:
        return reinterpret_cast<InstBinOpImm*>(reinterpret_cast<void*>(this) - node_offset)->GetDstReg();
        break;
    }

    UNREACHABLE()
}

uint32_t InstNode::GetSrcReg1()
{
    Type type = GetType();
    if (type == Type::InstControlJmp || type == Type::InstControlRet) {
        Inst::throw_error("No SrcReg1 in ", GetOpcode());
    }
    size_t node_offset = offsetof(Inst, inst_node);
    switch (type)
    {
    case Type::InstBinOp:
        return reinterpret_cast<InstBinOp*>(reinterpret_cast<void*>(this) - node_offset)->GetSrcReg1();
        break;
    case Type::InstBinOpImm:
        return reinterpret_cast<InstBinOpImm*>(reinterpret_cast<void*>(this) - node_offset)->GetSrcReg1();
        break;
    case Type::InstUtil:
        return reinterpret_cast<InstUtil*>(reinterpret_cast<void*>(this) - node_offset)->GetSrcReg1();
        break;
    case Type::InstUtilImm:
        return reinterpret_cast<InstUtilImm*>(reinterpret_cast<void*>(this) - node_offset)->GetSrcReg1();
        break;
    }

    UNREACHABLE()
}

uint32_t InstNode::GetSrcReg2()
{
    Type type = GetType();
    if (type != Type::InstBinOp && type != Type::InstUtil) {
        Inst::throw_error("No SrcReg2 in ", GetOpcode());
    }
    size_t node_offset = offsetof(Inst, inst_node);
    switch (type)
    {
    case Type::InstBinOp:
        return reinterpret_cast<InstBinOp*>(reinterpret_cast<void*>(this) - node_offset)->GetSrcReg2();
        break;
    case Type::InstUtil:
        return reinterpret_cast<InstUtil*>(reinterpret_cast<void*>(this) - node_offset)->GetSrcReg2();
        break;
    }

    UNREACHABLE()
}

uint32_t InstNode::GetBbId()
{
    Type type = GetType();
    if (type != Type::InstControlJmp) {
        Inst::throw_error("No BbId in ", GetOpcode());
    }
    size_t node_offset = offsetof(Inst, inst_node);
    return reinterpret_cast<InstControlJmp*>(reinterpret_cast<void*>(this) - node_offset)->GetBbId();
}

uint32_t InstNode::GetRetValReg()
{
    Type type = GetType();
    if (type != Type::InstControlRet) {
        Inst::throw_error("No RetValReg in ", GetOpcode());
    }
    size_t node_offset = offsetof(Inst, inst_node);
    return reinterpret_cast<InstControlRet*>(reinterpret_cast<void*>(this) - node_offset)->GetRetValReg();
}

uint32_t InstNode::GetImm()
{
    Type type = GetType();
    if (type != Type::InstBinOpImm && type != Type::InstUtilImm) {
        Inst::throw_error("No Imm in ", GetOpcode());
    }
    size_t node_offset = offsetof(Inst, inst_node);
    switch (type)
    {
    case Type::InstBinOpImm:
        return reinterpret_cast<InstBinOpImm*>(reinterpret_cast<void*>(this) - node_offset)->GetImm();
        break;
    case Type::InstUtilImm:
        return reinterpret_cast<InstUtilImm*>(reinterpret_cast<void*>(this) - node_offset)->GetImm();
        break;
    }

    UNREACHABLE()
}
