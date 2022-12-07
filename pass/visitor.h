#ifndef VISITOR_H
#define VISITOR_H

#include "ir/opcode.h"

class InstVisitor {
public:
    #define VISIT_FUNC(name, type)                                          \
    static void Visit##name(Inst *inst) { VisitDefault(); }

    OPCODE_LIST(VISIT_FUNC)
    #undef VISIT_FUNC

    static void VisitDefault()
    {
        return;
    }
};

#endif // VISITOR_H