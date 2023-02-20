#ifndef UTILS_H
#define UTILS_H

#include <algorithm>
#include <iostream>
#include <vector>

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
        throw_error("No field named ##field_name");                                                                    \
    }                                                                                                                  \
    virtual void Set##accessor_name(type accessor_name)                                                                \
    {                                                                                                                  \
        throw_error("No field named ##field_name");                                                                    \
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

void throw_inst_error(const std::string& msg, Opcode op_in);
void throw_error(const std::string& msg);

// Subtraction of two vectors: result = first - second
template <typename T>
std::vector<T> ComputeVectorsDiff(const std::vector<T>& first, const std::vector<T>& second)
{
    // this function can be implemented using std::set_difference with O(n*logn) complexity
    std::vector<T> result;
    for (auto item_first: first) {
        if (std::find(second.begin(), second.end(), item_first) == second.end()) {
            result.push_back(item_first);
        }
    }

    return result;
}

#endif // UTILS_H
