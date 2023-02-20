#ifndef OPCODES_H
#define OPCODES_H

#define TYPE_LIST(FUNC)                                                                                                \
    FUNC(InstWithTwoInputs)                                                                                            \
    FUNC(InstWithOneInput)                                                                                             \
    FUNC(InstJmp)                                                                                                      \
    FUNC(InstPhi)                                                                                                      \
    FUNC(InstParameter)                                                                                                \
    FUNC(InstConstant)                                                                                                 \
    FUNC(InstCall)                                                                                                     \
    FUNC(InstWithNoInputsUsers)

#define OPCODE_LIST(FUNC)                                                                                              \
    /* Arithmetic */                                                                                                   \
    FUNC(ADD, InstWithTwoInputs)                                                                                       \
    FUNC(SUB, InstWithTwoInputs)                                                                                       \
    FUNC(MUL, InstWithTwoInputs)                                                                                       \
    FUNC(DIV, InstWithTwoInputs)                                                                                       \
    FUNC(SHR, InstWithTwoInputs)                                                                                       \
    FUNC(SHL, InstWithTwoInputs)                                                                                       \
    FUNC(XOR, InstWithTwoInputs)                                                                                       \
    FUNC(NOT, InstWithOneInput)                                                                                        \
    /* Control */                                                                                                      \
    FUNC(JMP, InstJmp)                                                                                                 \
    FUNC(JA, InstJmp)                                                                                                  \
    FUNC(RET, InstWithOneInput)                                                                                        \
    FUNC(RET_VOID, InstWithNoInputsUsers)                                                                              \
    FUNC(THROW, InstWithNoInputsUsers)                                                                                 \
    FUNC(CATCH, InstWithNoInputsUsers)                                                                                 \
    FUNC(PARAMETER, InstParameter)                                                                                     \
    FUNC(CALL_STATIC, InstCall)                                                                                        \
    /* Utils */                                                                                                        \
    FUNC(MOV, InstWithOneInput)                                                                                        \
    FUNC(CMP, InstWithTwoInputs) /* TODO This inst has no users*/                                                      \
    FUNC(CAST, InstWithOneInput)                                                                                       \
    FUNC(PHI, InstPhi)                                                                                                 \
    FUNC(CONSTANT, InstConstant)

enum class Opcode
{
#define INIT_OPCODES(name, type) name,
    OPCODE_LIST(INIT_OPCODES)
#undef INIT_OPCODES
    DEFAULT,
    SIZE
};

enum class Type
{
#define INIT_TYPES(type) type,
    TYPE_LIST(INIT_TYPES)
#undef INIT_TYPES
    DEFAULT,
    SIZE
};

#endif // OPCODES_H
