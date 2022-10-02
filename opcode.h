#ifndef OPCODES_H
#define OPCODES_H

#define TYPE_LIST(FUNC)         \
    FUNC(InstBinOp)             \
    FUNC(InstBinOpImm)          \
    FUNC(InstControlJmp)        \
    FUNC(InstControlRet)        \
    FUNC(InstUtil)              \
    FUNC(InstUtilImm)           \


#define OPCODE_LIST(FUNC)       \
/* Arithmetic */                \
    FUNC(ADD, InstBinOp)        \
    FUNC(SUB, InstBinOp)        \
    FUNC(MUL, InstBinOp)        \
    FUNC(DIV, InstBinOp)        \
    FUNC(ADDI, InstBinOpImm)    \
    FUNC(SUBI, InstBinOpImm)    \
    FUNC(MULI, InstBinOpImm)    \
    FUNC(DIVI, InstBinOpImm)    \
/* Control */                   \
    FUNC(JMP, InstControlJmp)   \
    FUNC(JA, InstControlJmp)    \
    FUNC(RET, InstControlRet)   \
/* Utils */                     \
    FUNC(MOV, InstUtil)         \
    FUNC(MOVI, InstUtilImm)     \
    FUNC(CMP, InstUtil)         \
    FUNC(U32TOU64, InstUtil)    \

enum class Opcode {
    DEFAULT,
    #define INIT_OPCODES(name, type) name,
    OPCODE_LIST(INIT_OPCODES)
    #undef INIT_OPCODES
};

enum class Type {
    DEFAULT,
    #define INIT_TYPES(type) type,
    TYPE_LIST(INIT_TYPES)
    #undef INIT_TYPES
};

#endif // OPCODES_H
