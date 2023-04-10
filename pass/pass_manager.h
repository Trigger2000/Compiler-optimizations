#ifndef PASS_MANAGER_H
#define PASS_MANAGER_H

#include <tuple>
#include <type_traits>

class Graph;

class RPO;
class DomTreeSlow;
class DomTreeFast;
class LoopAnalyzer;
class ConstFolding;
class DCE;
class Peephole;
class Inlining;
class LivenessAnalysis;

using PassList = std::tuple<RPO, DomTreeSlow, DomTreeFast, LoopAnalyzer,
                            ConstFolding, DCE, Peephole, Inlining, LivenessAnalysis>;

class PassManager {
protected:
    template <typename Pass>
    void RunPass(Graph *g);

    template <typename Pass>
    constexpr size_t GetPassIndex();
private:
    template <typename Pass, size_t Index>
    constexpr size_t GetPassIndexHelper();
};

template <typename Pass>
void PassManager::RunPass(Graph *g)
{
    if (g->template IsPassValid<Pass>()) {
        return;
    }
    Pass pass;
    pass.RunPassImpl(g);
    g->template SetPassValidity<Pass>(true);
}

template <typename Pass>
constexpr size_t PassManager::GetPassIndex() {
    GetPassIndexHelper<Pass, 0>();
}

template <typename Pass, size_t Index>
constexpr size_t PassManager::GetPassIndexHelper() {
    static_assert(Index < std::tuple_size_v<PassList>);
    if constexpr (std::is_same_v<Pass, std::tuple_element_t<Index, PassList>>) {
        return Index;
    } else {
        return GetPassIndexHelper<Pass, Index + 1>();
    }
}

#endif // PASS_MANAGER_H
