#ifndef CONST_FOLDING_H
#define CONST_FOLDING_H

#include "ir/graph.h"
#include "visitor.h"

class ConstFolding : public InstVisitor {
public:
    void RunPassImpl(Graph *g);

private:
    static void VisitSUB(Inst *inst);
    static void VisitSHR(Inst *inst);
    static void VisitXOR(Inst *inst);

    static bool CheckConstInput(InstWithTwoInputs *inst);
    static void CreateNewConstant(Inst* old_inst, int32_t constant);

    #define BUILD_DISPATCH_TABLE(name, type)    \
    Visit##name,

    static constexpr std::array<void (*)(Inst*),
               static_cast<size_t>(Opcode::SIZE)> table_{OPCODE_LIST(BUILD_DISPATCH_TABLE)};
    #undef BUILD_DISPATCH_TABLE
};

#endif // CONST_FOLDING_H
