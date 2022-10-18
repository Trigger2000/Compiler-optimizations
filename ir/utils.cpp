#include "opcode.h"
#include "utils.h"

void throw_inst_error(std::string msg, Opcode op_in)
{
#define ERROR_INST(name, type)                                                                                         \
    if (Opcode::name == op_in) {                                                                                       \
        std::cerr << msg << #name << std::endl;                                                                        \
        std::abort();                                                                                                  \
    }
    OPCODE_LIST(ERROR_INST)
#undef ERROR_INST
}