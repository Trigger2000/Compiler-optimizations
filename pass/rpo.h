#ifndef RPO_H
#define RPO_H

#include "pass.h"


class RPO {
public:
    void RunPassImpl(Graph *g);

private:
    void PostOrderVisitor(std::vector<BasicBlock*>& result, BasicBlock* current)
    {
        if (current->GetMarker(BB_VISITED_MARKER) || current->GetMarker(BB_DELETED_MARKER)) {
            return;
        }
        current->SetMarker(BB_VISITED_MARKER, true);
        for (auto succ: current->GetSuccs()) {
            PostOrderVisitor(result, std::get<BasicBlock*>(succ));
        }
        result.push_back(current);
    }

    const int BB_VISITED_MARKER = 0;
    const int BB_DELETED_MARKER = 1;

    std::vector<BasicBlock*> basic_blocks_;
};

#endif // RPO_H
