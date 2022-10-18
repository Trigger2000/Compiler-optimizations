#ifndef UTILS_H
#define UTILS_H

#include <iostream>

#define UNREACHABLE()                                                                                                  \
    do {                                                                                                               \
        std::cerr << "this line should be unreachable" << std::endl;                                                   \
        std::cerr << __FILE__ << " " << __LINE__ << std::endl;                                                         \
    } while (0);

#define ACCESSOR_MUTATOR(field_name, accessor_name, type)                                                              \
    type Get##accessor_name()                                                                                          \
    {                                                                                                                  \
        return field_name;                                                                                             \
    }                                                                                                                  \
    void Set##accessor_name(type accessor_name)                                                                        \
    {                                                                                                                  \
        field_name = accessor_name;                                                                                    \
    }

#define ACCESSOR_MUTATOR_VIRTUAL(field_name, accessor_name, type)                                                      \
    virtual type Get##accessor_name()                                                                                  \
    {                                                                                                                  \
        UNREACHABLE()                                                                                                  \
    }                                                                                                                  \
    virtual void Set##accessor_name(type accessor_name)                                                                \
    {                                                                                                                  \
        UNREACHABLE()                                                                                                  \
    }                                                                                                                  \
    virtual bool Has##accessor_name()                                                                                  \
    {                                                                                                                  \
        return false;                                                                                                  \
    }

#define ACCESSOR_MUTATOR_OVERRIDE(field_name, accessor_name, type)                                                     \
    type Get##accessor_name() override                                                                                 \
    {                                                                                                                  \
        return field_name;                                                                                             \
    }                                                                                                                  \
    void Set##accessor_name(type accessor_name) override                                                               \
    {                                                                                                                  \
        field_name = accessor_name;                                                                                    \
    }                                                                                                                  \
    bool Has##accessor_name() override                                                                                 \
    {                                                                                                                  \
        return true;                                                                                                   \
    }

void throw_inst_error(std::string msg, Opcode op_in);

#endif // UTILS_H
