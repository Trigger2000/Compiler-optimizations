#ifndef OPCODES_H
#define OPCODES_H

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

#endif // OPCODES_H