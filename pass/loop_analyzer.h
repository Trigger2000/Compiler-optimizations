#ifndef LOOP_ANALYZER_H
#define LOOP_ANALYZER_H

#include "ir/graph.h"
#include "ir/loop.h"

class LoopAnalyzer {
public:
    void RunPassImpl(Graph *g);

private:
    void CollectBackEdges();
    void ProccessEdge(BasicBlock *root, BasicBlock *prev);
    void PopulateLoops();
    void LoopSearch(Loop *loop, BasicBlock* block);
    void BuildLoopTree();

    Graph *g_;
    marker black_marker_;
    marker gray_marker_;
};

#endif // LOOP_ANALYZER_H
