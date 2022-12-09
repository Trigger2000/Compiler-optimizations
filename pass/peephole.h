#ifndef PEEPHOLES_H
#define PEEPHOLES_H

#include "ir/graph.h"
#include "visitor.h"

class Peephole : public InstVisitor {
public:
    void RunPassImpl(Graph *g);

private:
    static void VisitSUB(Inst *inst);
    static void VisitSHR(Inst *inst);
    static void VisitXOR(Inst *inst);

    static void ProcessUsersInputs(Inst * old_inst, Inst *new_inst);

    #define BUILD_DISPATCH_TABLE(name, type)    \
    Visit##name,

    static constexpr std::array<void (*)(Inst*),
               static_cast<size_t>(Opcode::SIZE)> table_{OPCODE_LIST(BUILD_DISPATCH_TABLE)};
    #undef BUILD_DISPATCH_TABLE
};

#endif // PEEPHOLES_H